#include "Mouse.h"

#include <iostream>

#include "InputConversionMaps.h"

void Mouse::init() {
    for (auto& mouseButtonState : mouseButtonStates) {
        mouseButtonState = INPUT_STATE_UP;
    }
    justDownMouseButtons.clear();
    justUpMouseButtons.clear();
}

bool Mouse::processMouseEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        mouseButtonStates[e.button.button - 1] = INPUT_STATE_JUST_BECAME_DOWN;
        justDownMouseButtons.push_back(e.button.button - 1);
    }
    else if (e.type == SDL_MOUSEBUTTONUP) {
        mouseButtonStates[e.button.button - 1] = INPUT_STATE_JUST_BECAME_UP;
        justUpMouseButtons.push_back(e.button.button - 1);
    }
    else if (e.type == SDL_MOUSEMOTION) {
        mousePosition = glm::vec2(e.motion.x, e.motion.y);
    }
    else if (e.type == SDL_MOUSEWHEEL) {
        mouseScrollThisFrame = e.wheel.preciseY;
    }
    else {
        return false;
    }
    return true;
}

void Mouse::lateUpdate() {
    for (const int& button : justDownMouseButtons) {
        mouseButtonStates[button] = INPUT_STATE_DOWN;
    }
    justDownMouseButtons.clear();

    for (const int& button : justUpMouseButtons) {
        mouseButtonStates[button] = INPUT_STATE_UP;
    }
    justUpMouseButtons.clear();

    mouseScrollThisFrame = 0;
}

bool Mouse::getMouseButton(int button)
{
    // SDL Mouse button indices start at 1
    if (button >= 1 && button < NUMBER_OF_MOUSE_BUTTONS) {
        return mouseButtonStates[button - 1] == INPUT_STATE_DOWN || mouseButtonStates[button - 1] == INPUT_STATE_JUST_BECAME_DOWN;
    }
    else {
        // Invalid mouse button
        return false;
    }
}

bool Mouse::getMouseButtonByString(const std::string& buttonName) {
    const int button = stringToMouseButton(buttonName);
    if (button == -1) {
        return false;
    }
    return getMouseButton(button);
}

bool Mouse::getMouseButtonDown(int button)
{
    // SDL Mouse button indices start at 1
    if (button >= 1 && button < NUMBER_OF_MOUSE_BUTTONS) {
        return mouseButtonStates[button - 1] == INPUT_STATE_JUST_BECAME_DOWN;
    }
    else {
        // Invalid mouse button
        return false;
    }
}

bool Mouse::getMouseButtonDownByString(const std::string& buttonName) {
    const int button = stringToMouseButton(buttonName);
    if (button == -1) {
        return false;
    }
    return getMouseButtonDown(button);
}

bool Mouse::getMouseButtonUp(int button)
{
    // SDL Mouse button indices start at 1
    if (button >= 1 && button < NUMBER_OF_MOUSE_BUTTONS) {
        return mouseButtonStates[button - 1] == INPUT_STATE_JUST_BECAME_UP;
    }
    else {
        // Invalid mouse button
        return false;
    }
}

bool Mouse::getMouseButtonUpByString(const std::string& buttonName) {
    const int button = stringToMouseButton(buttonName);
    if (button == -1) {
        return false;
    }
    return getMouseButtonUp(button);
}

glm::vec2 Mouse::getMousePosition()
{
    return mousePosition;
}

float Mouse::getMouseScrollDelta()
{
    return mouseScrollThisFrame;
}

int Mouse::stringToMouseButton(const std::string& buttonName) {
    const std::string buttonNameLower = StringToLower(buttonName);
    const auto it = button_to_sdl_mouse_button.find(buttonNameLower);
    if (it == button_to_sdl_mouse_button.end()) {
        std::cerr << "Invalid mouse button: " << buttonName << "\n";
        return -1;
    }
    return it->second;
}
