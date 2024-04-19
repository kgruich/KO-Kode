#include "RenderRequests.h"

TextRenderRequest::TextRenderRequest() {
    text = "";
    fontName = "";
    color = { 0, 0, 0, 0 };
    fontSize = 0;
    x = 0;
    y = 0;
}

TextRenderRequest::TextRenderRequest(const std::string& text, const int x, const int y, const std::string& fontName, const int fontSize, const int r, const int g, const int b, const int a) {
    this->text = text;
    this->fontName = fontName;
    this->color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    this->fontSize = fontSize;
    this->x = x;
    this->y = y;
}

bool TextRenderRequest::operator==(const TextRenderRequest& other) const {
    return text == other.text
        && fontName == other.fontName
        && fontSize == other.fontSize;
}

ImageRenderRequest::ImageRenderRequest() {
    image = "";
    color = { 0, 0, 0, 0 };
    sortingOrder = 0;
    rotationDegrees = 0;
    x = 0;
    y = 0;
    scaleX = 1;
    scaleY = 1;
    pivotX = 0.5f;
    pivotY = 0.5f;
}

ImageRenderRequest::ImageRenderRequest(const std::string& image, const float x, const float y) {
    this->image = image;
    this->color = { 255, 255, 255, 255 };
    this->sortingOrder = 0;
    this->rotationDegrees = 0;
    this->x = x;
    this->y = y;
    this->pivotX = 0.5f;
    this->pivotY = 0.5f;
    this->scaleX = 1;
    this->scaleY = 1;
}

ImageRenderRequest::ImageRenderRequest(const std::string& image, const float x, const float y, const int rotationDegrees, const float scaleX, const float scaleY, const float pivotX, const float pivotY, int r, int g, int b, int a, int sortingOrder) {
    this->image = image;
    this->color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    this->sortingOrder = sortingOrder;
    this->rotationDegrees = rotationDegrees;
    this->x = x;
    this->y = y;
    this->pivotX = pivotX;
    this->pivotY = pivotY;
    this->scaleX = scaleX;
    this->scaleY = scaleY;
}

UIRenderRequest::UIRenderRequest() {
    image = "";
    color = {0, 0, 0, 0};
    sortingOrder = 0;
    x = 0;
    y = 0;
}

UIRenderRequest::UIRenderRequest(const std::string& image, const int x, const int y) {
    this->image = image;
    this->color = {255, 255, 255, 255};
    this->sortingOrder = 0;
    this->x = x;
    this->y = y;
}

UIRenderRequest::UIRenderRequest(const std::string& image, const int x, const int y, const int r, const int g, const int b, const int a, const int sortingOrder) {
    this->image = image;
    this->color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
    this->sortingOrder = sortingOrder;
    this->x = x;
    this->y = y;
}

PixelRenderRequest::PixelRenderRequest() {
    color = { 0, 0, 0, 0 };
    x = 0;
    y = 0;
}

PixelRenderRequest::PixelRenderRequest(const int x, const int y, const int r, const int g, const int b, const int a) {
    this->color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    this->x = x;
    this->y = y;
}