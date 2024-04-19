#ifndef INPUTCONVERSIONMAPS_H
#define INPUTCONVERSIONMAPS_H
#include <unordered_map>
#include <string>
#include "SDL.h"

inline std::string GetEventName(const Uint32 eventType) {
	switch (eventType) {
	case SDL_QUIT: return "SDL_QUIT";
	case SDL_WINDOWEVENT: return "SDL_WINDOWEVENT";
	case SDL_KEYDOWN: return "SDL_KEYDOWN";
	case SDL_KEYUP: return "SDL_KEYUP";
	case SDL_MOUSEBUTTONDOWN: return "SDL_MOUSEBUTTONDOWN";
	case SDL_MOUSEBUTTONUP: return "SDL_MOUSEBUTTONUP";
	case SDL_MOUSEMOTION: return "SDL_MOUSEMOTION";
	case SDL_CONTROLLERDEVICEADDED: return "SDL_CONTROLLERDEVICEADDED";
	case SDL_CONTROLLERDEVICEREMOVED: return "SDL_CONTROLLERDEVICEREMOVED";
	case SDL_CONTROLLERBUTTONDOWN: return "SDL_CONTROLLERBUTTONDOWN";
	case SDL_CONTROLLERBUTTONUP: return "SDL_CONTROLLERBUTTONUP";
	case SDL_CONTROLLERAXISMOTION: return "SDL_CONTROLLERAXISMOTION";
		// ... add other case statements as needed for other SDL events ...
	default: return std::to_string(eventType);
	}
}

const std::unordered_map<std::string, Uint8> button_to_sdl_mouse_button = {
	{"left", SDL_BUTTON_LEFT},
	{"middle", SDL_BUTTON_MIDDLE},
	{"right", SDL_BUTTON_RIGHT},
	{"x1", SDL_BUTTON_X1},
	{"x2", SDL_BUTTON_X2}
};

const std::unordered_map<std::string, SDL_GameControllerAxis> axis_to_sdl_controller_axis = {
	// Thumbstick axes
	{"left_stick_x", SDL_CONTROLLER_AXIS_LEFTX},  // Left thumbstick horizontal axis
	{"left_stick_y", SDL_CONTROLLER_AXIS_LEFTY},  // Left thumbstick vertical axis
	{"right_stick_x", SDL_CONTROLLER_AXIS_RIGHTX}, // Right thumbstick horizontal axis
	{"right_stick_y", SDL_CONTROLLER_AXIS_RIGHTY}, // Right thumbstick vertical axis

	// Trigger axes
	{"left_trigger", SDL_CONTROLLER_AXIS_TRIGGERLEFT},  // Left trigger
	{"right_trigger", SDL_CONTROLLER_AXIS_TRIGGERRIGHT}, // Right trigger
};

const std::unordered_map<std::string, SDL_GameControllerButton> button_to_sdl_controller_button = {
	// Face (action) buttons
	{"a", SDL_CONTROLLER_BUTTON_A},
	{"b", SDL_CONTROLLER_BUTTON_B},
	{"x", SDL_CONTROLLER_BUTTON_X},
	{"y", SDL_CONTROLLER_BUTTON_Y},
	// D-pad buttons
	{"dpad_up", SDL_CONTROLLER_BUTTON_DPAD_UP},
	{"dpad_down", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
	{"dpad_left", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
	{"dpad_right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
	// Shoulder buttons
	{"leftshoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
	{"rightshoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
	// Start / Select (Options / Menu) buttons
	{"start", SDL_CONTROLLER_BUTTON_START},
	{"back", SDL_CONTROLLER_BUTTON_BACK},
	{"guide", SDL_CONTROLLER_BUTTON_GUIDE},
	// Thumbstick buttons
	{"leftstick", SDL_CONTROLLER_BUTTON_LEFTSTICK},
	{"rightstick", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
	// Misc
	{"misc1", SDL_CONTROLLER_BUTTON_MISC1}, // Used for other buttons/keys not covered by the constants here
	// System buttons
	{"paddle1", SDL_CONTROLLER_BUTTON_PADDLE1}, // Paddle 1 (Xbox Elite paddle)
	{"paddle2", SDL_CONTROLLER_BUTTON_PADDLE2}, // Paddle 2 (Xbox Elite paddle)
	{"paddle3", SDL_CONTROLLER_BUTTON_PADDLE3}, // Paddle 3 (Xbox Elite paddle)
	{"paddle4", SDL_CONTROLLER_BUTTON_PADDLE4}, // Paddle 4 (Xbox Elite paddle)
	{"touchpad", SDL_CONTROLLER_BUTTON_TOUCHPAD}, // PS4/PS5 touchpad button
};

const std::unordered_map<std::string, SDL_Scancode> keycode_to_scancode = {
	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};
#endif // INPUTCONVERSIONMAPS_H
