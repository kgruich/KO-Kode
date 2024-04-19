#include "FontDB.h"
#include <filesystem>
#include <iostream>


FontDB::FontDB() {}

FontDB* FontDB::getInstance()
{
    return instance;
}

void FontDB::init() {
    instance = this;

    TTF_Init();
    initialized = true;
}

void FontDB::initCheck() {
    if (!initialized) {
        //std::cerr << "error: never initialized FontDB";
        exit(1);
    }
}

TTF_Font* FontDB::getFont(const std::string& fontName, const int fontSize) {
    initCheck();
    return initFont(fontName, fontSize);
}

TTF_Font* FontDB::initFont(const std::string& fontName, const int fontSize) {
    initCheck();

    // Check if font is already loaded
    if (const auto& fontNameIt = fonts.find(fontName); fontNameIt != fonts.end()) {
        if (const auto& fontSizeIt = fontNameIt->second.find(fontSize); fontSizeIt != fontNameIt->second.end()) {
            return fontSizeIt->second;
        }
    }

    // Check if font folder even exists
    if (!std::filesystem::is_directory(fontFolder)) {
        //std::cerr << "fontFolder doesn't exist or isn't a directory";
        exit(1);
    }

    // Check if font file exists
    std::filesystem::path fontPath = fontFolder + (fontName + ".ttf");
    if (!std::filesystem::exists(fontPath)) {
        std::cout << "error: font " + fontName + " missing";
        exit(0);
    }

    // Load font and cache
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), fontSize);
    if (font == nullptr) {
        std::cout << "error: failed to load font " + fontName;
        exit(0);
    }
    fonts[fontName][fontSize] = font;
    return font;
}