#include "Controller.h"

#include <iostream>

#include "Input.h"

Controller::Controller(SDL_GameController* rawController) : controller(rawController, SDL_GameControllerClose) {
    justDownButtons.clear();
    justUpButtons.clear();
    for (int button = 0; button < NUM_BUTTONS; button++) {
        buttonStates[button] = INPUT_STATE_UP;
    }

    std::fill(axisStates.begin(), axisStates.end(), 0);

    // Determine the number of touchpads, and then resize touchpadFingerStates accordingly
    int numTouchpads = SDL_GameControllerGetNumTouchpads(controller.get());
    touchpadFingerStates.resize(numTouchpads);
    for (int touchpad = 0; touchpad < numTouchpads; ++touchpad) {
        const int numFingers = SDL_GameControllerGetNumTouchpadFingers(controller.get(), touchpad);
        touchpadFingerStates[touchpad].resize(numFingers);
    }

    printControllerInfo();
}

Controller::~Controller() {
    controller.reset();
    buttonStates.fill(INPUT_STATE_UP);
    axisStates.fill(0);
    justDownButtons.clear();
    justUpButtons.clear();
}

void Controller::lateUpdate() {
    for (const auto& button : justDownButtons) {
        buttonStates[button] = INPUT_STATE_DOWN;
    }
    justDownButtons.clear();

    for (const auto& button : justUpButtons) {
        buttonStates[button] = INPUT_STATE_UP;
    }
    justUpButtons.clear();
}

void Controller::setButtonState(const inputState input, const SDL_GameControllerButton button) {
    buttonStates[button] = input;
}

bool Controller::getButton(const SDL_GameControllerButton button) const {
    if (buttonStates[button] == INPUT_STATE_DOWN || buttonStates[button] == INPUT_STATE_JUST_BECAME_DOWN) {
        return true;
    }
    else {
        return false;
    }
}

bool Controller::getButtonByString(const std::string& buttonName) const {
    const SDL_GameControllerButton button = stringToButton(buttonName);
    if (button != SDL_CONTROLLER_BUTTON_INVALID) {
        return getButton(button);
    }
    std::cerr << "Invalid button name: " << buttonName << "\n";
    return false;
}

bool Controller::getButtonDown(const SDL_GameControllerButton button) const {
    if (buttonStates[button] == INPUT_STATE_JUST_BECAME_DOWN) {
        return true;
    }
    else {
        return false;
    }
}

bool Controller::getButtonDownByString(const std::string& buttonName) const {
    const SDL_GameControllerButton button = stringToButton(buttonName);
    if (button != SDL_CONTROLLER_BUTTON_INVALID) {
        return getButtonDown(button);
    }
    std::cerr << "Invalid button name: " << buttonName << "\n";
    return false;
}

bool Controller::getButtonUp(const SDL_GameControllerButton button) const {
    if (buttonStates[button] == INPUT_STATE_JUST_BECAME_UP) {
        return true;
    }
    else {
        return false;
    }
}

bool Controller::getButtonUpByString(const std::string& buttonName) const {
    const SDL_GameControllerButton button = stringToButton(buttonName);
    if (button != SDL_CONTROLLER_BUTTON_INVALID) {
        return getButtonUp(button);
    }
    std::cerr << "Invalid button name: " << buttonName << "\n";
    return false;
}

bool Controller::setDeadzoneInt(const int deadzone) {
    if (deadzone < 0 || deadzone > AXIS_MAX_VALUE) {
        std::cerr << "Deadzone must be between 0 and 32767 as a int\n";
        return false;
    }
    else {
        this->deadzone = deadzone;
        return true;
    }
}

bool Controller::setDeadzoneFloat(const float deadzone) {
    if (deadzone < 0.0f || deadzone > 1.0f) {
        std::cerr << "Deadzone must be between 0 and 1 as a float\n";
        return false;
    }
    else {
        this->deadzone = static_cast<int>(AXIS_MAX_VALUE * deadzone);
        return true;
    }
}

void Controller::setAxisState(const SDL_GameControllerAxis axis, const int value) {
    axisStates[axis] = value;
}

float Controller::getAxisValue(const SDL_GameControllerAxis axis, const bool useDeadzone) const {
    // Check against deadzone
    if (useDeadzone) {
        if (std::abs(axisStates[axis]) < deadzone) {
            return 0.0f; // The stick is in the deadzone return 0.0f as no movement
        }
    }
    // Return normalized value (if out of deadzone or deadzone is not used)
    return static_cast<float>(axisStates[axis]) / AXIS_MAX_VALUE;
}

float Controller::getAxisValueByString(const std::string& axisName, const bool useDeadzone) const {
    const SDL_GameControllerAxis axis = stringToAxis(axisName);
    if (axis != SDL_CONTROLLER_AXIS_INVALID) {
        return getAxisValue(axis, useDeadzone);
    }
    std::cerr << "Invalid axis name: " << axisName << "\n";
    return float();
}

bool Controller::getAxisPastThreshold(const SDL_GameControllerAxis axis, const float threshold, const bool useDeadzone) const {
    const float axisValue = getAxisValue(axis, useDeadzone);
    if (threshold < 0) {
        return axisValue < threshold; // For negative thresholds, check if axis value is less than threshold
    }
    else {
        return axisValue > threshold; // For positive thresholds, check if axis value is greater than threshold
    }
}

bool Controller::getAxisPastThresholdByString(const std::string& axisName, const float threshold, const bool useDeadzone) const {
    const SDL_GameControllerAxis axis = stringToAxis(axisName);
    if (axis != SDL_CONTROLLER_AXIS_INVALID) {
        return getAxisPastThreshold(axis, threshold, useDeadzone);
    }
    std::cerr << "Invalid axis name: " << axisName << "\n";
    return false;
}

void Controller::printControllerInfo() const {
    const char* controllerName = SDL_GameControllerName(controller.get());
    if (!controllerName) {
        controllerName = "unknown controller";  // Fallback in case SDL_GameControllerName returns a null pointer
    }
    std::cerr << "Added " << controllerName << " controller\n";

    // Print additional diagnostic information
    std::cerr << "Mapping: " << SDL_GameControllerMapping(controller.get()) << "\n";
    std::cerr << "Vendor: " << SDL_GameControllerGetVendor(controller.get()) << "\n";
    std::cerr << "Product: " << SDL_GameControllerGetProduct(controller.get()) << "\n";
    std::cerr << "Product Version: " << SDL_GameControllerGetProductVersion(controller.get()) << "\n";

    // Check supported buttons
    for (int i = 0; i < NUM_BUTTONS; ++i) {
        const SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(i);
        const char* buttonName = SDL_GameControllerGetStringForButton(button);
        bool supported = SDL_GameControllerHasButton(controller.get(), button);
        std::cerr << "Button: " << buttonName << ", Supported: " << (supported ? "Yes" : "No") << "\n";
    }

    // Check supported axes
    for (int i = 0; i < NUM_AXES; ++i) {
        const SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(i);
        const char* axisName = SDL_GameControllerGetStringForAxis(axis);
        bool supported = SDL_GameControllerHasAxis(controller.get(), axis);
        std::cerr << "Axis: " << axisName << ", Supported: " << (supported ? "Yes" : "No") << "\n";
    }

    // Check supported touchpads
    for (int i = 0; i < touchpadFingerStates.size(); ++i) {
        std::cerr << "Touchpad: " << i + 1 << ", supports " << touchpadFingerStates[i].size() << " fingers\n";
    }
}

SDL_GameControllerButton Controller::stringToButton(const std::string& buttonName) {
    const std::string buttonNameLower = StringToLower(buttonName);
    const auto it = button_to_sdl_controller_button.find(buttonNameLower);
    if (it == button_to_sdl_controller_button.end()) {
        std::cerr << "Invalid button name: " << buttonNameLower << "\n";
        return SDL_CONTROLLER_BUTTON_INVALID;
    }
    else {
        return it->second;
    }
}
SDL_GameControllerAxis Controller::stringToAxis(const std::string& axisName) {
    const std::string axisNameLower = StringToLower(axisName);
    const auto it = axis_to_sdl_controller_axis.find(axisNameLower);
    if (it == axis_to_sdl_controller_axis.end()) {
        std::cerr << "Invalid axis name: " << axisNameLower << "\n";
        return SDL_CONTROLLER_AXIS_INVALID;
    }
    else {
        return it->second;
    }
}

void Controller::setTouchpadFingerState(const size_t touchpad, const size_t finger, const float x, const float y, const float pressure,
                                        const inputState state) {
    if (isTouchpadFingerValid(touchpad + 1, finger + 1)) {
        // Update the finger state based on the event
        touchpadFingerStates[touchpad][finger].x = x;
        touchpadFingerStates[touchpad][finger].y = y;
        touchpadFingerStates[touchpad][finger].pressure = pressure;
        touchpadFingerStates[touchpad][finger].state = state;
    }
}

bool Controller::getTouchpadFinger(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
        return false;
    }
    return getTouchpadFingerState(touchpad, finger)->state == INPUT_STATE_DOWN || getTouchpadFingerState(touchpad, finger)->state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Controller::getTouchpadFingerDown(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
                   return false;
    }
    return getTouchpadFingerState(touchpad, finger)->state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Controller::getTouchpadFingerUp(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
        return false;
    }
    //TODO: consider if this should be INPUT_STATE_JUST_BECAME_UP || INPUT_STATE_UP
    return getTouchpadFingerState(touchpad, finger)->state == INPUT_STATE_JUST_BECAME_UP;
}

float Controller::getTouchpadFingerX(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
        return 0.0f;
    }
    return getTouchpadFingerState(touchpad, finger)->x;
}

float Controller::getTouchpadFingerY(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
        return 0.0f;
    }
    return getTouchpadFingerState(touchpad, finger)->y;
}

float Controller::getTouchpadFingerPressure(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadFingerValid(touchpad, finger)) {
        return 0.0f;
    }
    return getTouchpadFingerState(touchpad, finger)->pressure;
}

// Modifiers for converting to 1-indexed lua
const TouchpadFinger* Controller::getTouchpadFingerState(const size_t touchpad, const size_t finger) const {
    return &touchpadFingerStates[touchpad - 1][finger - 1];
}

bool Controller::isTouchpadValid(const size_t touchpad) const {
    return touchpad - 1 < touchpadFingerStates.size();
}

bool Controller::isTouchpadFingerValid(const size_t touchpad, const size_t finger) const {
    if (!isTouchpadValid(touchpad)) {
        return false;
    }
    bool retval = finger - 1 < touchpadFingerStates[touchpad - 1].size();
    return retval;
}

bool Controller::isConnected() const {
    return SDL_GameControllerGetAttached(controller.get());
}
