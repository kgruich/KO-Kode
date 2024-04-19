#ifndef CONTROLLERMANAGER_H
#define CONTROLLERMANAGER_H
#include <memory>
#include <unordered_map>

#include "Controller.h"
#include "SDL2/SDL.h"

class ControllerManager
{
public:
	// Initialize controller
	static void initController();
	// Process events for controller
	static bool processControllerEvent(const SDL_Event& e);
	// Update controller states at end of frame
	static void lateUpdate();

	// Add controller
	static void addController(int joystickIndex);
	// Remove controller
	static void removeController(SDL_JoystickID id);

	// Get controller with joystick id
	static Controller* getControllerById(SDL_JoystickID id);
	// Get controller with joystick index
	static Controller* getControllerByIndex(int index);
	// Get total number of controllers
	static size_t getControllerCount();

	// Get a button state from a specific or all controllers
	static bool getControllerButton(SDL_GameControllerButton buttonName, int playerId = -1);
	static bool getControllerButtonDown(SDL_GameControllerButton controllerButton, int playerId = -1);
	static bool getControllerButtonUp(SDL_GameControllerButton controllerButton, int playerId = -1);

	// Get if an axis is past a threshold from a specific or all controllers
	static bool getControllerAxisPastThreshold(SDL_GameControllerAxis axisName, float threshold = 0.5, bool useDeadzone = true, int playerId = -1);

private:
    static inline std::unordered_map<SDL_JoystickID, std::unique_ptr<Controller>> controllers = {};
    static inline std::unordered_map<int, SDL_JoystickID> indexToJoystickID;
};
#endif