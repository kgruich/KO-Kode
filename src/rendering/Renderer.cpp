#include "Renderer.h"
#include <glm/common.hpp>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "../actors/ComponentManager.h"

void Renderer::init(const ResourcesDB& configDB) {
    if (instance != nullptr) {
        throw std::logic_error("Renderer already initialized");
    }
    instance = this;
    resolution.x = configDB.mainDoc.getInt("x_resolution", 640);
    resolution.y = configDB.mainDoc.getInt("y_resolution", 360);
    zoomFactor = configDB.mainDoc.getFloat("zoom_factor", 1.0);

    const auto windowName = configDB.mainDoc.getCharPointer("game_title", "");
    window = Helper::SDL_CreateWindow498(windowName.get(), 100, 100, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
    renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    float offsetX = configDB.mainDoc.getFloat("cam_offset_x", 0.0);
    float offsetY = configDB.mainDoc.getFloat("cam_offset_y", 0.0);
    zoomFactor = configDB.mainDoc.getFloat("zoom_factor", 1.0);

    clearColor.r = configDB.mainDoc.getInt("clear_color_r", 255);
    clearColor.g = configDB.mainDoc.getInt("clear_color_g", 255);
    clearColor.b = configDB.mainDoc.getInt("clear_color_b", 255);
    clearColor.a = configDB.mainDoc.getInt("clear_color_a", 255);
    clear();

    // Add relevent functions to Lua API
    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginNamespace("Text")
        .addFunction("Draw", &Renderer::queueText)
        .endNamespace();
    
    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginNamespace("Image")
        .addFunction("DrawUI", &Renderer::queueUI)
        .addFunction("DrawUIEx", &Renderer::queueUIExtended)
        .addFunction("Draw", &Renderer::queueImage)
        .addFunction("DrawEx", &Renderer::queueImageExtended)
        .addFunction("DrawPixel", &Renderer::queuePixel)
        .endNamespace();

    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginNamespace("Camera")
        .addFunction("SetPosition", &Renderer::setCameraPosition)
        .addFunction("SetZoom", &Renderer::setCameraZoom)
        .addFunction("GetPositionX", &Renderer::getCameraPositionX)
        .addFunction("GetPositionY", &Renderer::getCameraPositionY)
        .addFunction("GetZoom", &Renderer::getCameraZoom)
        .endNamespace();

    loadImages();
    fontDB->init();
}

void Renderer::shutDown() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

Renderer* Renderer::getInstance()
{
    if (instance == nullptr) {
        throw std::logic_error("Call initialize() before getInstance()");
    }
    return instance;
}

Renderer::~Renderer() {
    for (auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();

    for (auto& pair : textTextureCache) {
        SDL_DestroyTexture(pair.second);
    }
    textTextureCache.clear();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}

void Renderer::loadImages() {
    if (!std::filesystem::exists(imagePath)) {
        return;
    }
    for (const auto& file : std::filesystem::directory_iterator(imagePath)) {
        if (file.path().extension() == ".png") {
            std::string imageName = file.path().stem().string();
            std::string filePath = file.path().string();
            textureCache[imageName] = IMG_LoadTexture(Renderer::getRenderer(), filePath.c_str());
        }
    }
}

void Renderer::render() {
    clear();
    std::stable_sort(imageRenderQueue.begin(), imageRenderQueue.end(), ImageRenderRequest::compareImageRenderRequests);
    std::stable_sort(UIRenderQueue.begin(), UIRenderQueue.end(), UIRenderRequest::compareUIRenderRequests);
    setRenderScale(zoomFactor);

    for (auto& request : imageRenderQueue) {
        renderImage(request);
    }
    imageRenderQueue.clear();

    setRenderScale(1);
    for (auto& request : UIRenderQueue) {
        renderUI(request);
    }
    UIRenderQueue.clear();

    for (auto& request : textRenderQueue) {
        renderText(request);
    }
    textRenderQueue.clear();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (auto& request : pixelRenderQueue) {
        renderPixel(request);
    }
    pixelRenderQueue.clear();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    Helper::SDL_RenderPresent498(renderer);
}

void Renderer::queueImage(const std::string& image, const float x, const float y) {
    imageRenderQueue.emplace_back(image, x, y);
}

void Renderer::queueImageExtended(const std::string& image, const float x, const float y, const float rotationDegrees, const float scaleX, const float scaleY, const float pivotX, const float pivotY, float r, float g, float b, float a, float sortingOrder) {
    imageRenderQueue.emplace_back(image, x, y, static_cast<int>(rotationDegrees), scaleX, scaleY, pivotX, pivotY, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), static_cast<int>(sortingOrder));
}

void Renderer::queueUI(const std::string& image, const float x, const float y) {
    UIRenderQueue.emplace_back(image, static_cast<int>(x), static_cast<int>(y));
}

void Renderer::queueUIExtended(const std::string& image, const float x, const float y, const float r, const float g, const float b, const float a, const float sortingOrder) {
    UIRenderQueue.emplace_back(image, static_cast<int>(x), static_cast<int>(y), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sortingOrder);
}

void Renderer::queueText(const std::string& text, const float x, const float y, const std::string& fontName, const float fontSize, const float r, const float g, const float b, const float a) {
    textRenderQueue.emplace_back(text, static_cast<int>(x), static_cast<int>(y), fontName, static_cast<int>(fontSize), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
}

void Renderer::queuePixel(const float x, const float y, const float r, const float g, const float b, const float a) {
    pixelRenderQueue.emplace_back(static_cast<int>(x), static_cast<int>(y), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
}

// Rendering Functions

void Renderer::renderImage(ImageRenderRequest& request) {
    constexpr int pixelsPerUnit = 100;

    // Correct for camera position
    glm::vec2 finalRenderPosition = glm::vec2(request.x, request.y) - cameraPosition;

    // Get texture and its width/height
    SDL_Texture* texture = getTexture(request.image);
    SDL_Rect dest{};
    SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);

    // Apply scale
    SDL_RendererFlip flip = getFlip({ request.scaleX, request.scaleY });
    float xScale = std::abs(request.scaleX);
    float yScale = std::abs(request.scaleY);
    dest.w *= xScale;
    dest.h *= yScale;

    // Calculate pivot point, pivotX/Y are normalized to [0, 1]
    SDL_Point pivotPoint = { static_cast<int>(request.pivotX * dest.w), static_cast<int>(request.pivotY * dest.h) };

    dest.x = static_cast<int>(finalRenderPosition.x * pixelsPerUnit + resolution.x * 0.5f * (1.0f / zoomFactor) - pivotPoint.x);
    dest.y = static_cast<int>(finalRenderPosition.y * pixelsPerUnit + resolution.y * 0.5f * (1.0f / zoomFactor) - pivotPoint.y);

    // Apply tint/alpha to texture
    SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
    SDL_SetTextureAlphaMod(texture, request.color.a);

    Helper::SDL_RenderCopyEx498(0, "", renderer, texture, nullptr, &dest, static_cast<double>(request.rotationDegrees), &pivotPoint, SDL_FLIP_NONE);

    // Remove tint/alpha from texture
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, 255);
}

void Renderer::renderUI(UIRenderRequest& request) {
    const auto texture = getTexture(request.image);

    SDL_Rect dest{};
    dest.x = request.x;
    dest.y = request.y;
    SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
    SDL_Point pivotPoint = { 0, 0 };

    // Apply tint/alpha to texture
    SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
    SDL_SetTextureAlphaMod(texture, request.color.a);

    Helper::SDL_RenderCopyEx498(0, "", renderer, texture, nullptr, &dest, 0, &pivotPoint, SDL_FLIP_NONE);

    // Remove tint/alpha from texture
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, 255);
}

void Renderer::renderText(TextRenderRequest& request) {
    const auto texture = getTexture(request);

    SDL_Rect dest{};
    dest.x = request.x;
    dest.y = request.y;

    SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);

    SDL_Point pivotPoint = { 0, 0 };

    Helper::SDL_RenderCopyEx498(0, "", renderer, texture, nullptr, &dest, 0, &pivotPoint, SDL_FLIP_NONE);
}

void Renderer::renderPixel(PixelRenderRequest& request) {
    SDL_SetRenderDrawColor(renderer, request.color.r, request.color.g, request.color.b, request.color.a);
    SDL_RenderDrawPoint(renderer, request.x, request.y);
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(renderer);
}

// Utility Functions

void Renderer::setCameraPosition(const float x, const float y) {
    cameraPosition = { x, y };
}

void Renderer::setCameraZoom(const float zoom) {
    zoomFactor = zoom;
}

float Renderer::getCameraPositionX() {
    return cameraPosition.x;
}

float Renderer::getCameraPositionY() {
    return cameraPosition.y;
}

float Renderer::getCameraZoom() {
    return zoomFactor;
}

SDL_Texture* Renderer::getTexture(const std::string& image) {
    const auto& it = textureCache.find(image);
    if (it != textureCache.end()) {
        return it->second;
    }
    std::cout << "error: missing image " + image;
    exit(0);
}

SDL_Texture* Renderer::getTexture(const TextRenderRequest& request) {
    const auto& it = textTextureCache.find(request);
    if (it != textTextureCache.end()) {
        return it->second;
    }
    const auto font = FontDB::getInstance()->getFont(request.fontName, request.fontSize);
    const auto textSurface = TTF_RenderText_Solid(font, request.text.c_str(), request.color);
    textTextureCache[request] = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return textTextureCache[request];
}

SDL_Renderer* Renderer::getRenderer() {
    return renderer;
}

glm::ivec2 Renderer::getResolution()
{
    return resolution;
}

SDL_RendererFlip Renderer::getFlip(const glm::vec2& scaleFactor)
{
    int flip = SDL_FLIP_NONE;

    if (scaleFactor.x < 0.0f) {
        flip |= SDL_FLIP_HORIZONTAL;
    }

    if (scaleFactor.y < 0.0f) {
        flip |= SDL_FLIP_VERTICAL;
    }

    return static_cast<SDL_RendererFlip>(flip);
}


void Renderer::setRenderScale(float scaleFactor)
{
    SDL_RenderSetScale(renderer, scaleFactor, scaleFactor);
}
