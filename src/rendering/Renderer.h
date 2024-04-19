#ifndef RENDERER_H
#define RENDERER_H

#include "SDL2/SDL.h"
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_float2.hpp>
#include "../external_helpers/Helper.h"
#include "../databases/ResourcesDB.h"
#include "FontDB.h"
#include "RenderRequests.h"

struct Color {
    int r, g, b, a;
};

class Renderer
{
public:
    // ---------- Initialization Functions ----------

    void init(const ResourcesDB& configDB);
    static void shutDown();
    static Renderer* getInstance();
    static SDL_Renderer* getRenderer();

    // ---------- Core Rendering Functions ----------

    static void render();

    // ---------- Render Request Functions ----------

    // Image.Draw
    static void queueImage(const std::string& image, const float x, const float y);
    // Image.DrawEx: color + tint/alpha + sortingOrder
    static void queueImageExtended(const std::string& image, const float x, const float y, const float rotationDegrees, const float scaleX, const float scaleY, const float pivotX, const float pivotY, float r, float g, float b, float a, float sortingOrder);

    // Image.DrawUI
    static void queueUI(const std::string& image, const float x, const float y);
    // Image.DrawUIEx: color + tint/alpha + sortingOrder
    static void queueUIExtended(const std::string& image, const float x, const float y, const float r, const float g, const float b, const float a, const float sortingOrder);

    // Text.Draw
    static void queueText(const std::string& text, const float x, const float y, const std::string& fontName, const float fontSize, const float r, const float g, const float b, const float a);
    // Image.DrawPixel
    static void queuePixel(const float x, const float y, const float r, const float g, const float b, const float a);

    // -------------- Utility Functions -------------

    static void setCameraPosition(const float x, const float y);
    static void setCameraZoom(const float zoom);

    static float getCameraPositionX();
    static float getCameraPositionY();
    static float getCameraZoom();

private:
    static inline Renderer* instance = nullptr;
    static inline SDL_Window* window = nullptr;
    static inline SDL_Renderer* renderer = nullptr;
    static inline Color clearColor = { 255, 255, 255, 255 };

    static inline glm::ivec2 resolution = {0, 0};
    static inline glm::vec2 cameraPosition = {0, 0};
    static inline float zoomFactor = 1.0f;

    static inline FontDB* fontDB = nullptr;

    static inline std::string imagePath = "resources/images/";

    static inline std::vector<ImageRenderRequest> imageRenderQueue = {};
    static inline std::vector<UIRenderRequest> UIRenderQueue = {};
    static inline std::vector<TextRenderRequest> textRenderQueue = {};
    static inline std::vector<PixelRenderRequest> pixelRenderQueue = {};

    static inline std::unordered_map<std::string, SDL_Texture*> textureCache = {};
    static inline std::unordered_map<TextRenderRequest, SDL_Texture*, TextRenderRequestHash> textTextureCache = {};

    // ---------- Initialization Functions ----------

    Renderer() = default;
    ~Renderer();
    static void loadImages();

    // ---------- Core Rendering Functions ----------

    static void renderImage(ImageRenderRequest& request);
    static void renderUI(UIRenderRequest& request);
    static void renderText(TextRenderRequest& request);
    static void renderPixel(PixelRenderRequest& request);
    static void clear();

    // -------------- Utility Functions -------------

    static SDL_Texture* getTexture(const std::string& image);
    static SDL_Texture* getTexture(const TextRenderRequest& request);

    static void setRenderScale(float scaleFactor);

    static glm::ivec2 getResolution();
    static SDL_RendererFlip getFlip(const glm::vec2& scaleFactor);
};

#endif // RENDERER_H