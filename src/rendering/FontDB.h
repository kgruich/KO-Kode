#ifndef FONTDB_H
#define FONTDB_H

#include "SDL2_ttf/SDL_ttf.h"
#include <unordered_map>
#include <string>

class FontDB
{
public:
    FontDB();
    static FontDB* getInstance();
    void init();

    static TTF_Font* getFont(const std::string& fontName, const int fontSize);

private:
    static inline FontDB* instance = nullptr;
    static inline bool initialized = false;

    static inline std::string fontFolder = "resources/fonts/";
    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts = {};

    static void initCheck();
    static TTF_Font* initFont(const std::string& fontName, const int fontSize);
};
#endif