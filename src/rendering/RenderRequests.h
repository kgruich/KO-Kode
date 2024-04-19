#ifndef RENDERREQUESTS_H
#define RENDERREQUESTS_H

#include <string>
#include <SDL2/SDL.h>

class ImageRenderRequest {
public:
    std::string image;
    SDL_Color color;
    int sortingOrder;
    int rotationDegrees;
    float x;
    float y;
    float scaleX;
    float scaleY;
    float pivotX;
    float pivotY;

    // Default ctor
    ImageRenderRequest();

    // Image.Draw ctor (still needs pivots calculated)
    ImageRenderRequest(const std::string& image, const float x, const float y);

    // Image.DrawEx ctor
    ImageRenderRequest(const std::string& image, const float x, const float y, const int rotationDegrees, const float scaleX, const float scaleY, const float pivotX, const float pivotY, int r, int g, int b, int a, int sortingOrder);

    static bool compareImageRenderRequests(const ImageRenderRequest& lhs, const ImageRenderRequest& rhs) {
        return lhs.sortingOrder < rhs.sortingOrder;
    }
};

// Hash function for ImageRenderRequest, only image since everything else doesn't affect SDL_Texture reuse
//struct ImageRenderRequestHash {
//    std::size_t operator()(const ImageRenderRequest& irr) const {
//        std::size_t h1 = std::hash<std::string>()(irr.image);
//        return h1;
//    }
//};

class UIRenderRequest {
public:
    std::string image;
    SDL_Color color;
    int sortingOrder;
    int x;
    int y;

    // Default ctor
    UIRenderRequest();

    // Image.DrawUI ctor
    UIRenderRequest(const std::string& image, const int x, const int y);

    // Image.DrawUIEx ctor
    UIRenderRequest(const std::string& image, const int x, const int y, const int r, const int g, const int b, const int a, const int sortingOrder);

    static bool compareUIRenderRequests(const UIRenderRequest& lhs, const UIRenderRequest& rhs) {
        return lhs.sortingOrder < rhs.sortingOrder;
    }
};

class TextRenderRequest {
public:
    std::string text;
    std::string fontName;
    SDL_Color color;
    int fontSize;
    int x;
    int y;

    TextRenderRequest();
    TextRenderRequest(const std::string& text, const int x, const int y, const std::string& fontName, const int fontSize, const int r, const int g, const int b, const int a);

    // For comparing two TextRenderRequests to know if they are cached, x and y can be different since its not cached
    bool operator==(const TextRenderRequest& other) const;
};

// Hash function for ImageRenderRequest, excludes x and y since those don't affect SDL_Texture reuse
struct TextRenderRequestHash {
    std::size_t operator()(const TextRenderRequest& trr) const {
        std::size_t h1 = std::hash<std::string>()(trr.text);
        std::size_t h2 = std::hash<std::string>()(trr.fontName);
        std::size_t h3 = std::hash<int>()(trr.fontSize);

        // Combine the hashes. This is a simple way to combine hash values,
        // but in production code consider using a robust technique like Boost's hash_combine.
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class PixelRenderRequest {
public:
    SDL_Color color;
    int x;
    int y;

    // Default ctor
    PixelRenderRequest();

    // Image.DrawPixel ctor
    PixelRenderRequest(const int x, const int y, const int r, const int g, const int b, const int a);
};

#endif