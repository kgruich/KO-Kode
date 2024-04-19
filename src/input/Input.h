#ifndef INPUT_H
#define INPUT_H

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "SDL2/SDL.h"
#include "glm/vec2.hpp"
#include <unordered_map>
#include <vector>

#include "ControllerManager.h"
#include "Keyboard.h"
#include "Mouse.h"

class Input
{
public:
	// Call this at the beginning of the game
	static void init();
	// Call this at the beginning of the event loop
	static void processEvent(const SDL_Event& e);
	// Call this at the very end of the frame
	static void lateUpdate();

	static bool removeActionBinding(const std::string& actionName);

	static bool addInputBindingWrapper(const std::string& actionName, const std::string& inputType,
		const std::string& inputName, lua_State* L);

	static bool addInputBinding(const std::string& actionName, const std::string& inputType, const std::string& inputName, int playerId = -1, float axisThreshold = 0.5, bool useDeadzone = true);

	static bool removeInputBinding(const std::string& actionName, const std::string& inputType, const std::string& inputName);

	static bool toggleActionInputType(const std::string& actionName, const std::string& inputType);

	static bool resetAllActionBindings();

	static bool getAction(const std::string& actionName);

private:
	static inline std::unordered_map<std::string, ActionBinding> actionMap;

	static void saveActionMapToFile(const std::string& filename);

	static void loadActionMapFromFile(const std::string& filename);

	static InputBinding::Type getInputTypeFromString(const std::string& inputType);
};

#endif