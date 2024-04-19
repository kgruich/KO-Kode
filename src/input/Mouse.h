#ifndef MOUSE_H
#define MOUSE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>

#include "InputUtils.h"
#include "SDL2/SDL.h"

constexpr size_t NUMBER_OF_MOUSE_BUTTONS = SDL_BUTTON_X2 + 1; // Up to SDL_BUTTON_X2

class Mouse
{
public:
	// Initialize mouse states
	static void init();
	// Process events for mouse
	static bool processMouseEvent(const SDL_Event& e);
	// Update mouse states at end of frame
	static void lateUpdate();

	// Is a certain mouse button down currently?
	static bool getMouseButton(int button);
	static bool getMouseButtonByString(const std::string& buttonName);

	// Was a certain mouse button pressed down this frame?
	static bool getMouseButtonDown(int button);
	static bool getMouseButtonDownByString(const std::string& buttonName);

	// Was a certain mouse button released this frame?
	static bool getMouseButtonUp(int button);
	static bool getMouseButtonUpByString(const std::string& buttonName);

	// Get the mouse position
	static glm::vec2 getMousePosition();
	// Get the mouse scroll delta
	static float getMouseScrollDelta();

	static int stringToMouseButton(const std::string& buttonName);
private:
	// Mouse buttons
	static inline inputState mouseButtonStates[NUMBER_OF_MOUSE_BUTTONS];
	static inline std::vector<int> justDownMouseButtons;
	static inline std::vector<int> justUpMouseButtons;

	// Mouse other
	static inline glm::vec2 mousePosition;
	static inline float mouseScrollThisFrame = 0;
};
#endif