#include "ControllerManager.h"

#include <iostream>

void ControllerManager::initController() {
    controllers.clear();
    indexToJoystickID.clear();
    //std::cerr << "SDL_HINT_JOYSTICK_HIDAPI_PS5: " << (SDL_GetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5) ? SDL_GetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5) : "not set") << '\n';
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    //SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    // Add all connected controllers
    //for (int i = 0; i < SDL_NumJoysticks(); i++) {
    //    if (SDL_IsGameController(i)) {
    //        addController(i);
    //    }
    //}
}

bool ControllerManager::processControllerEvent(const SDL_Event& e) {
    switch (e.type) {
    case SDL_CONTROLLERDEVICEADDED:
        addController(e.cdevice.which); // index of new controller
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        removeController(e.cdevice.which); // instance id of removed controller
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (controllers.find(e.cbutton.which) != controllers.end()) {
            controllers[e.cbutton.which]->setButtonState(INPUT_STATE_JUST_BECAME_DOWN, static_cast<SDL_GameControllerButton>(e.cbutton.button)); // instance id of removed controller
            std::cerr << "Button " << SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(e.cbutton.button)) << " pressed down\n";
        }
        break;
    case SDL_CONTROLLERBUTTONUP:
        if (controllers.find(e.cbutton.which) != controllers.end()) {
            controllers[e.cbutton.which]->setButtonState(INPUT_STATE_JUST_BECAME_UP, static_cast<SDL_GameControllerButton>(e.cbutton.button)); // instance id of removed controller
            std::cerr << "Button " << SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(e.cbutton.button)) << " released\n";
        }
        break;
    case SDL_CONTROLLERAXISMOTION:
        if (controllers.find(e.cbutton.which) != controllers.end()) {
            controllers[e.caxis.which]->setAxisState(static_cast<SDL_GameControllerAxis>(e.caxis.axis), e.caxis.value); // instance id of removed controller
            //std::cerr << "Axis " << static_cast<SDL_GameControllerAxis>(e.caxis.axis) << " moved to " << e.caxis.value << "\n";
        }
        break;
    case SDL_CONTROLLERTOUCHPADDOWN:
        if (controllers.find(e.ctouchpad.which) != controllers.end()) {
            inputState state = INPUT_STATE_JUST_BECAME_DOWN;
            controllers[e.ctouchpad.which]->setTouchpadFingerState(
                e.ctouchpad.touchpad, e.ctouchpad.finger,
                e.ctouchpad.x, e.ctouchpad.y,
                e.ctouchpad.pressure, state);
            //std::cerr << "Touchpad " << e.ctouchpad.touchpad << " finger " << e.ctouchpad.finger << " pressed down\n";
        }
        break;
    case SDL_CONTROLLERTOUCHPADUP:
        if (controllers.find(e.ctouchpad.which) != controllers.end()) {
            inputState state = INPUT_STATE_JUST_BECAME_UP;
            controllers[e.ctouchpad.which]->setTouchpadFingerState(
                e.ctouchpad.touchpad, e.ctouchpad.finger,
                e.ctouchpad.x, e.ctouchpad.y,
                e.ctouchpad.pressure, state);
            //std::cerr << "Touchpad " << e.ctouchpad.touchpad << " finger " << e.ctouchpad.finger << " released\n";
        }
        break;
    case SDL_CONTROLLERTOUCHPADMOTION:
        if (controllers.find(e.ctouchpad.which) != controllers.end()) {
            inputState state = INPUT_STATE_DOWN; // The finger is moving, therefore it's still down
            controllers[e.ctouchpad.which]->setTouchpadFingerState(
                e.ctouchpad.touchpad, e.ctouchpad.finger,
                e.ctouchpad.x, e.ctouchpad.y,
                e.ctouchpad.pressure, state);
            //std::cerr << "Touchpad " << e.ctouchpad.touchpad << " finger " << e.ctouchpad.finger << " moved to " << e.ctouchpad.x << ", " << e.ctouchpad.y << "\n";
        }
        break;
    default:
        return false;
    }
    return true; // Event was processed
}

void ControllerManager::lateUpdate() {
    for (const auto& [id, controller] : controllers) {
        controller->lateUpdate();
    }
}

void ControllerManager::addController(const int joystickIndex) {
    if (SDL_IsGameController(joystickIndex)) {
        SDL_GameController* rawControllerPtr = SDL_GameControllerOpen(joystickIndex);
        if (rawControllerPtr) {
            SDL_Joystick* joy = SDL_GameControllerGetJoystick(rawControllerPtr);
            SDL_JoystickID joyID = SDL_JoystickInstanceID(joy);
            controllers[joyID] = std::make_unique<Controller>(rawControllerPtr);
            indexToJoystickID[joystickIndex] = joyID; // Store the mapping of index to JoystickID

            if (!controllers[joyID]->isConnected()) {
                std::cerr << "Controller is not attached.\n";
                exit(1);
            }
        }
        else {
            std::cerr << "Failed to open game controller: " << SDL_GetError() << "\n";
            exit(1);
        }
    }
}

void ControllerManager::removeController(const SDL_JoystickID id) {
    auto iter = controllers.find(id);
    if (iter != controllers.end()) {
        // Remove index-to-ID mapping
        for (auto it = indexToJoystickID.begin(); it != indexToJoystickID.end(); ++it) {
            if (it->second == id) {
                indexToJoystickID.erase(it);
                break;
            }
        }
        // Remove the controller
        controllers.erase(iter);
        std::cerr << "Removed controller with ID: " << id << "\n";
    }
}

Controller* ControllerManager::getControllerById(const SDL_JoystickID id) {
    const auto it = controllers.find(id);
    if (it == controllers.end()) {
        return nullptr;
    }
    else {
        return it->second.get();
    }
}

Controller* ControllerManager::getControllerByIndex(const int index) {
    const auto it = indexToJoystickID.find(index - 1);
    if (it == indexToJoystickID.end()) {
        return nullptr;
    }
    else {
        //std::cerr << "Found controller with ID: " << it->second << "\n";
        return getControllerById(it->second);
    }
}

size_t ControllerManager::getControllerCount() { return controllers.size(); }

bool ControllerManager::getControllerButton(const SDL_GameControllerButton buttonName, const int playerId) {
    if (playerId == -1) {
        for (const auto& [id, controller] : controllers) {
            if (controller->getButton(buttonName)) { return true; }
        }
    }
    else {
        Controller* controller = getControllerByIndex(playerId);
        if (controller) { return controller->getButton(buttonName); }
    }
    return false;
}

bool ControllerManager::getControllerButtonDown(SDL_GameControllerButton controllerButton, int playerId) {
    if (playerId == -1) {
        for (const auto& [id, controller] : controllers) {
            if (controller->getButtonDown(controllerButton)) { return true; }
        }
    }
    else {
        Controller* controller = getControllerByIndex(playerId);
        if (controller) { return controller->getButtonDown(controllerButton); }
    }
    return false;
}

bool ControllerManager::getControllerButtonUp(SDL_GameControllerButton controllerButton, int playerId) {
    if (playerId == -1) {
        for (const auto& [id, controller] : controllers) {
            if (controller->getButtonUp(controllerButton)) { return true; }
        }
    }
    else {
        Controller* controller = getControllerByIndex(playerId);
        if (controller) { return controller->getButtonUp(controllerButton); }
    }
    return false;
}

bool ControllerManager::getControllerAxisPastThreshold(const SDL_GameControllerAxis axisName, const float threshold, const bool useDeadzone, const int playerId) {
    if (playerId == -1) {
        for (const auto& [id, controller] : controllers) {
            if (controller->getAxisPastThreshold(axisName, threshold, useDeadzone)) { return true; }
        }
    }
    else {
        Controller* controller = getControllerByIndex(playerId);
        if (controller) { return controller->getAxisPastThreshold(axisName, threshold, useDeadzone); }
    }
    return false;
}
