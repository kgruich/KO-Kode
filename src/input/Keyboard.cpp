#include "Keyboard.h"

#include <iostream>

#include "InputConversionMaps.h"

void Keyboard::init() {
    keyboardStates.clear();
    justDownScancodes.clear();
    justUpScancodes.clear();
    for (int keycode = SDL_SCANCODE_UNKNOWN; keycode < SDL_NUM_SCANCODES; keycode++) {
        keyboardStates[static_cast<SDL_Scancode>(keycode)] = INPUT_STATE_UP;
    }
}

bool Keyboard::processKeyboardEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        //std::cerr << e.key.keysym.scancode << " pressed down\n";
        keyboardStates[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        justDownScancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_KEYUP) {
        //std::cerr << e.key.keysym.scancode << " released\n";
        keyboardStates[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        justUpScancodes.push_back(e.key.keysym.scancode);
    }
    else {
        return false;
    }
    return true;
}

void Keyboard::lateUpdate() {
    for (const SDL_Scancode& scancode : justDownScancodes) {
        keyboardStates[scancode] = INPUT_STATE_DOWN;
    }
    justDownScancodes.clear();

    for (const SDL_Scancode& scancode : justUpScancodes) {
        keyboardStates[scancode] = INPUT_STATE_UP;
    }
    justUpScancodes.clear();
}

bool Keyboard::getKeyByScancode(SDL_Scancode keycode) {
    return keyboardStates[keycode] == INPUT_STATE_DOWN || keyboardStates[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Keyboard::getKeyByString(const std::string& keyname)
{
    const SDL_Scancode scancode = stringToScancode(keyname);
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        return false;
    }
    return getKeyByScancode(scancode);
}

bool Keyboard::getKeyDownByScancode(SDL_Scancode keycode) {
    return keyboardStates[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Keyboard::getKeyDownByString(const std::string& keyname)
{
    const SDL_Scancode scancode = stringToScancode(keyname);
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        return false;
    }
    return getKeyDownByScancode(scancode);
}

bool Keyboard::getKeyUpByScancode(SDL_Scancode keycode) {
    return keyboardStates[keycode] == INPUT_STATE_JUST_BECAME_UP;
}

bool Keyboard::getKeyUpByString(const std::string& keyname) {
    const SDL_Scancode scancode = stringToScancode(keyname);
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        return false;
    }
    return getKeyUpByScancode(scancode);
}

SDL_Scancode Keyboard::stringToScancode(const std::string& keyname) {
    const std::string keyNameLower = StringToLower(keyname);
    const auto it = keycode_to_scancode.find(keyNameLower);
    if (it == keycode_to_scancode.end()) {
        // Invalid keycode
        std::cerr << "Invalid keycode " << keyNameLower << "\n";
        return SDL_SCANCODE_UNKNOWN;
    }
    else {
        return it->second;
    }
}
