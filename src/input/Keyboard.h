#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include <unordered_map>
#include <vector>

#include "InputUtils.h"
#include "SDL2/SDL.h"

constexpr size_t NUMBER_OF_KEYS = SDL_NUM_SCANCODES; // Up to SDL_NUM_SCANCODES

class Keyboard
{
public:
	// Initialize keyboard states
	static void init();
	// Process events for keyboard
	static bool processKeyboardEvent(const SDL_Event& e);
	// Update keyboard states at end of frame
	static void lateUpdate();
	// Is a certain key down currently?
	static bool getKeyByScancode(SDL_Scancode keycode);
	static bool getKeyByString(const std::string& keyname);
	// Was a certain key pressed down this frame?
	static bool getKeyDownByScancode(SDL_Scancode keycode);
	static bool getKeyDownByString(const std::string& keyname);
	// Is a certain key released this frame?
	static bool getKeyUpByScancode(SDL_Scancode keycode);
	static bool getKeyUpByString(const std::string& keyname);

	static SDL_Scancode stringToScancode(const std::string& keyname);

private:
	static inline std::unordered_map<SDL_Scancode, inputState> keyboardStates;
	static inline std::vector<SDL_Scancode> justDownScancodes;
	static inline std::vector<SDL_Scancode> justUpScancodes;
};
#endif