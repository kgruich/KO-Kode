#include "Input.h"
#include <iostream>
#include "../actors/ComponentManager.h"

void Input::init() {
    Keyboard::init();
    Mouse::init();
    ControllerManager::initController();

    // Input Mouse & Keyboard API functions + GetController
    // TODO: When not working with autograder, move Keyboard and Mouse to their own namespace
    // TODO: When not working with autograder, add sensitivity & haptics API functions
    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginNamespace("Input")
        .addFunction("GetKey", &Keyboard::getKeyByString)
        .addFunction("GetKeyDown", &Keyboard::getKeyDownByString)
        .addFunction("GetKeyUp", &Keyboard::getKeyUpByString)
        .addFunction("GetMousePosition", &Mouse::getMousePosition)
        .addFunction("GetMouseButton", &Mouse::getMouseButton)
        .addFunction("GetMouseButtonDown", &Mouse::getMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Mouse::getMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Mouse::getMouseScrollDelta)
        .addFunction("GetController", &ControllerManager::getControllerByIndex)
        .addFunction("GetControllerCount", &ControllerManager::getControllerCount)
        .addFunction("GetAction", &Input::getAction)
        .addFunction("AddInputBinding", &Input::addInputBindingWrapper)
        .addFunction("RemoveInputBinding", &Input::removeInputBinding)
        .addFunction("ToggleActionInputType", &Input::toggleActionInputType)
        .addFunction("RemoveActionBinding", &Input::removeActionBinding)
        .addFunction("ResetAllActionBindings", &Input::resetAllActionBindings)
        .endNamespace();

    // Register Controller class with Lua
    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginClass<Controller>("controller")
        .addFunction("GetButton", &Controller::getButtonByString)
        .addFunction("GetButtonDown", &Controller::getButtonDownByString)
        .addFunction("GetButtonUp", &Controller::getButtonUpByString)
        .addFunction("SetDeadzoneInt", &Controller::setDeadzoneInt)
        .addFunction("SetDeadzoneFloat", &Controller::setDeadzoneFloat)
        .addFunction("GetAxisValue", &Controller::getAxisValueByString)
        .addFunction("GetAxisPastThreshold", &Controller::getAxisPastThresholdByString)
        .addFunction("IsTouchpadValid", &Controller::isTouchpadValid)
        .addFunction("isTouchpadFingerValid", &Controller::isTouchpadFingerValid)
        .addFunction("GetTouchpadFinger", &Controller::getTouchpadFinger)
        .addFunction("GetTouchpadFingerDown", &Controller::getTouchpadFingerDown)
        .addFunction("GetTouchpadFingerUp", &Controller::getTouchpadFingerUp)
        .addFunction("GetTouchpadFingerX", &Controller::getTouchpadFingerX)
        .addFunction("GetTouchpadFingerY", &Controller::getTouchpadFingerY)
        .addFunction("GetTouchpadFingerPressure", &Controller::getTouchpadFingerPressure)
        .endClass();
}

void Input::processEvent(const SDL_Event& e) {
    // Log all events for debugging
    //std::cerr << "Event type: " << GetEventName(e.type) << "\n";
    // Use weird if statements to avoid checking a known event for another event type
    if (Keyboard::processKeyboardEvent(e))
        return;
    else if (Mouse::processMouseEvent(e))
        return;
    else if (ControllerManager::processControllerEvent(e))
        return;
}

void Input::lateUpdate() {
    Keyboard::lateUpdate();
    Mouse::lateUpdate();
    ControllerManager::lateUpdate();
}

bool Input::removeActionBinding(const std::string& actionName) {
    const auto it = actionMap.find(actionName);
    if (it != actionMap.end()) {
        actionMap.erase(it);
        return true;
    }
    return false;
}

// This function is a wrapper for the addInputBinding function that is called from Lua
// used as a workaround to use default arguments in Lua
bool Input::addInputBindingWrapper(const std::string& actionName, const std::string& inputType,
    const std::string& inputName, lua_State* L) {
    // Check how many arguments are actually passed to set the defaults accordingly
    int playerId = static_cast<int>(luaL_optnumber(L, 4, -1));
    float axisThreshold = luaL_optnumber(L, 5, 0.0f);
    bool useDeadzone = lua_isnoneornil(L, 6) ? true : lua_toboolean(L, 6);

    return addInputBinding(actionName, inputType, inputName, playerId,
        axisThreshold, useDeadzone);
}

bool Input::addInputBinding(const std::string& actionName, const std::string& inputType,
                            const std::string& inputName, const int playerId, const float axisThreshold, const bool useDeadzone) {
    InputBinding newBinding;
    InputBinding::Type type = getInputTypeFromString(inputType);
    switch (type) {
    case InputBinding::KEYBOARD_DOWN:
    case InputBinding::KEYBOARD_JUST_DOWN:
    case InputBinding::KEYBOARD_JUST_UP: {
        const SDL_Scancode keycode = Keyboard::stringToScancode(inputName);
        if (keycode == SDL_SCANCODE_UNKNOWN) return false; // Key not found
        newBinding.type = type;
        newBinding.keyboardScancode = keycode;
        newBinding.playerId = -1; // Keyboard is not player specific
        break;
    }
    case InputBinding::MOUSE_BUTTON_DOWN:
    case InputBinding::MOUSE_BUTTON_JUST_DOWN:
    case InputBinding::MOUSE_BUTTON_JUST_UP: {
        // Try to find the mouse button in the map first
        int buttonIndex = Mouse::stringToMouseButton(inputName);
        if (buttonIndex != -1) {
            newBinding.type = type;
            newBinding.mouseButton = static_cast<Uint8>(buttonIndex);
        }
        else {
            // If not found, try to convert the string to an integer
            try {
                buttonIndex = std::stoi(inputName);
                if (buttonIndex < 1 || buttonIndex >= NUMBER_OF_MOUSE_BUTTONS) return false; // Invalid button index
                newBinding.type = type;
                newBinding.mouseButton = static_cast<Uint8>(buttonIndex);
            }
            catch (const std::invalid_argument& ia) {
                return false; // inputName was not a valid number
            }
            catch (const std::out_of_range& oor) {
                return false; // inputName was a number but it was too big
            }
        }
        newBinding.playerId = -1; // Mouse is not player specific
        break;
    }
    case InputBinding::CONTROLLER_BUTTON_DOWN:
    case InputBinding::CONTROLLER_BUTTON_JUST_DOWN:
    case InputBinding::CONTROLLER_BUTTON_JUST_UP: {
        const auto controllerButton = Controller::stringToButton(inputName);
        if (controllerButton == SDL_CONTROLLER_BUTTON_INVALID) return false; // Button not found

        newBinding.type = type;
        newBinding.controllerButton = controllerButton;
        newBinding.playerId = playerId;
        break;
    }
    case InputBinding::CONTROLLER_AXIS: {
        if (axisThreshold < -1.0f || axisThreshold > 1.0f) return false; // Invalid threshold (should be between 0 and 1)

        const auto axis = Controller::stringToAxis(inputName);
        if (axis == SDL_CONTROLLER_AXIS_INVALID) return false; // Axis not found

        newBinding.type = type;
        newBinding.controllerAxis = axis;
        newBinding.axisThreshold = axisThreshold;
        newBinding.useDeadzone = useDeadzone;
        newBinding.playerId = playerId;
        break;
    }
    case InputBinding::NONE: {
        return false; // Invalid input type
    }
    }

    actionMap[actionName].bindings.push_back(newBinding);
    return true;
}

bool Input::removeInputBinding(const std::string& actionName, const std::string& inputType,
    const std::string& inputName) {
    const auto it = actionMap.find(actionName);
    if (it == actionMap.end()) return false; // Action not found
    auto& bindings = it->second.bindings;
    const InputBinding::Type queryType = getInputTypeFromString(inputType);
    bool retval = false;
    for (auto bindingIt = bindings.begin(); bindingIt != bindings.end();) {
        if (bindingIt->type == queryType) {
            switch (bindingIt->type) {
            case InputBinding::KEYBOARD_DOWN:
                if (bindingIt->keyboardScancode == Keyboard::stringToScancode(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::KEYBOARD_JUST_DOWN:
                if (bindingIt->keyboardScancode == Keyboard::stringToScancode(inputName)) {
                    bindingIt = bindings.erase(bindingIt); \
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::KEYBOARD_JUST_UP:
                if (bindingIt->keyboardScancode == Keyboard::stringToScancode(inputName)) {
                    bindingIt = bindings.erase(bindingIt); \
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::MOUSE_BUTTON_DOWN:
                if (bindingIt->mouseButton == Mouse::stringToMouseButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::MOUSE_BUTTON_JUST_DOWN:
                if (bindingIt->mouseButton == Mouse::stringToMouseButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::MOUSE_BUTTON_JUST_UP:
                if (bindingIt->mouseButton == Mouse::stringToMouseButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::CONTROLLER_BUTTON_DOWN:
                if (bindingIt->controllerButton == Controller::stringToButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::CONTROLLER_BUTTON_JUST_DOWN:
                if (bindingIt->controllerButton == Controller::stringToButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::CONTROLLER_BUTTON_JUST_UP:
                if (bindingIt->controllerButton == Controller::stringToButton(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::CONTROLLER_AXIS:
                if (bindingIt->controllerAxis == Controller::stringToAxis(inputName)) {
                    bindingIt = bindings.erase(bindingIt);
                    retval = true;
                }
                else {
                    ++bindingIt;
                }
                break;
            case InputBinding::NONE:
                break;
            }
        }
    }
    return retval;
}

bool Input::toggleActionInputType(const std::string& actionName, const std::string& inputType) {
    const auto it = actionMap.find(actionName);
    if (it == actionMap.end()) return false; // Action not found
    ActionBinding& action = it->second;
    switch (getInputTypeFromString(inputType)) {
    case InputBinding::KEYBOARD_DOWN:
    case InputBinding::KEYBOARD_JUST_DOWN:
    case InputBinding::KEYBOARD_JUST_UP:
        action.useKeyboard = !action.useKeyboard;
        break;
    case InputBinding::MOUSE_BUTTON_DOWN:
    case InputBinding::MOUSE_BUTTON_JUST_DOWN:
    case InputBinding::MOUSE_BUTTON_JUST_UP:
        action.useMouse = !action.useMouse;
        break;
    case InputBinding::CONTROLLER_BUTTON_DOWN:
    case InputBinding::CONTROLLER_BUTTON_JUST_DOWN:
    case InputBinding::CONTROLLER_BUTTON_JUST_UP:
        action.useControllerButtons = !action.useControllerButtons;
        break;
    case InputBinding::CONTROLLER_AXIS:
        action.useControllerAxes = !action.useControllerAxes;
        break;
    case InputBinding::NONE:
        return false;
    }
    return true;
}

bool Input::resetAllActionBindings() {
    actionMap.clear();
    return true;
}

bool Input::getAction(const std::string& actionName) {
    const auto it = actionMap.find(actionName);
    if (it != actionMap.end()) {
        ActionBinding& action = it->second;
        for (const InputBinding& binding : action.bindings) {
            switch (binding.type) {
            case InputBinding::KEYBOARD_DOWN:
                if (action.useKeyboard && Keyboard::getKeyByScancode(binding.keyboardScancode))
                    return true;
                break;
            case InputBinding::KEYBOARD_JUST_DOWN:
                if (action.useKeyboard && Keyboard::getKeyDownByScancode(binding.keyboardScancode))
                    return true;
                break;
            case InputBinding::KEYBOARD_JUST_UP:
                if (action.useKeyboard && Keyboard::getKeyUpByScancode(binding.keyboardScancode))
                    return true;
                break;
            case InputBinding::MOUSE_BUTTON_DOWN:
                if (action.useMouse && Mouse::getMouseButton(binding.mouseButton))
                    return true;
                break;
            case InputBinding::MOUSE_BUTTON_JUST_DOWN:
                if (action.useMouse && Mouse::getMouseButtonDown(binding.mouseButton))
                    return true;
                break;
            case InputBinding::MOUSE_BUTTON_JUST_UP:
                if (action.useMouse && Mouse::getMouseButtonUp(binding.mouseButton))
                    return true;
                break;
            case InputBinding::CONTROLLER_BUTTON_DOWN:
                if (action.useControllerButtons && ControllerManager::getControllerButton(binding.controllerButton, binding.playerId))
                     return true;
                break;
            case InputBinding::CONTROLLER_BUTTON_JUST_DOWN:
                if (action.useControllerButtons && ControllerManager::getControllerButtonDown(binding.controllerButton, binding.playerId))
                    return true;
                break;
            case InputBinding::CONTROLLER_BUTTON_JUST_UP:
                if (action.useControllerButtons && ControllerManager::getControllerButtonUp(binding.controllerButton, binding.playerId))
                    return true;
                break;
            case InputBinding::CONTROLLER_AXIS:
                if (action.useControllerAxes && ControllerManager::getControllerAxisPastThreshold(binding.controllerAxis, binding.axisThreshold, binding.useDeadzone, binding.playerId))
                    return true;
                break;
            case InputBinding::NONE:
                break;
            }
        }
    }
    return false;
}

void Input::saveActionMapToFile(const std::string& filename) {
}

void Input::loadActionMapFromFile(const std::string& filename) {
}

InputBinding::Type Input::getInputTypeFromString(const std::string& inputType) {
    const std::string inputTypeLower = StringToLower(inputType);
    if (inputTypeLower == "keyboard") { return InputBinding::KEYBOARD_DOWN; }
    if (inputTypeLower == "keyboard_down") { return InputBinding::KEYBOARD_DOWN; }
    if (inputTypeLower == "keyboard_just_down") { return InputBinding::KEYBOARD_JUST_DOWN; }
    if (inputTypeLower == "keyboard_just_up") { return InputBinding::KEYBOARD_JUST_UP; }
    if (inputTypeLower == "mouse_button") { return InputBinding::MOUSE_BUTTON_DOWN; }
    if (inputTypeLower == "mouse_button_down") { return InputBinding::MOUSE_BUTTON_DOWN; }
    if (inputTypeLower == "mouse_button_just_down") { return InputBinding::MOUSE_BUTTON_JUST_DOWN; }
    if (inputTypeLower == "mouse_button_just_up") { return InputBinding::MOUSE_BUTTON_JUST_UP; }
    if (inputTypeLower == "controller_button") { return InputBinding::CONTROLLER_BUTTON_DOWN; }
    if (inputTypeLower == "controller_button_down") { return InputBinding::CONTROLLER_BUTTON_DOWN; }
    if (inputTypeLower == "controller_button_just_down") { return InputBinding::CONTROLLER_BUTTON_JUST_DOWN; }
    if (inputTypeLower == "controller_button_just_up") { return InputBinding::CONTROLLER_BUTTON_JUST_UP; }
    if (inputTypeLower == "controller_axis") { return InputBinding::CONTROLLER_AXIS; }
    std::cerr << "Invalid input type: " << inputTypeLower << "\n";
    return InputBinding::NONE; // Return NONE for invalid input types
}
