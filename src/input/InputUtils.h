#ifndef INPUTUTILS_H
#define INPUTUTILS_H
#include <algorithm>
#include <SDL.h>

enum inputState { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

struct InputBinding {
    enum Type { NONE, KEYBOARD_DOWN, KEYBOARD_JUST_DOWN, KEYBOARD_JUST_UP, MOUSE_BUTTON_DOWN, MOUSE_BUTTON_JUST_DOWN, MOUSE_BUTTON_JUST_UP, CONTROLLER_BUTTON_DOWN, CONTROLLER_BUTTON_JUST_DOWN, CONTROLLER_BUTTON_JUST_UP, CONTROLLER_AXIS } type;

    // Keep the union to store one of the possible input types.
    union {
        SDL_Scancode keyboardScancode;
        Uint8 mouseButton;
        SDL_GameControllerButton controllerButton;
        struct { // Only used for CONTROLLER_AXIS
            SDL_GameControllerAxis controllerAxis;
            float axisThreshold; // The threshold at which the axis is considered "down"
            bool useDeadzone; // Whether to use the controller's deadzone or not
        };
    };

    // For local actions, you can add a player identifier. -1 = global action, 0 = player 1, 1 = player 2, etc.
    int playerId;

    // Default constructor to make it a global action by default.
    InputBinding() : axisThreshold(0.5), useDeadzone(true), playerId(-1) {}

    // Additional data members might be included here for axes, like a threshold or direction.
};

struct ActionBinding {
    std::vector<InputBinding> bindings;
    bool useKeyboard = true;
    bool useMouse = true;
    bool useControllerButtons = true;
    bool useControllerAxes = true;
};

class TouchpadFinger {
public:
	inputState state;
	float x;
	float y;
	float pressure;
};

static inline std::string StringToLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

#endif