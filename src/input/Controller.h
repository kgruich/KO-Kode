#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL.h>
#include <array>
#include <memory>
#include <vector>
#include "InputConversionMaps.h"
#include "InputUtils.h"

constexpr size_t NUM_BUTTONS = SDL_CONTROLLER_BUTTON_MAX;

constexpr size_t NUM_AXES = SDL_CONTROLLER_AXIS_MAX;
constexpr int AXIS_MAX_VALUE = 32767;

class Controller {
public:
	Controller() = default;

	explicit Controller(SDL_GameController* rawController);

	~Controller();

	void lateUpdate();

	// --------------------- Buttons ---------------------

	// Method to be called when a button is pressed or released
	void setButtonState(inputState input, SDL_GameControllerButton button);

	bool getButton(SDL_GameControllerButton button) const;
	bool getButtonByString(const std::string& buttonName) const;

	bool getButtonDown(SDL_GameControllerButton button) const;
	bool getButtonDownByString(const std::string& buttonName) const;

	bool getButtonUp(SDL_GameControllerButton button) const;
	bool getButtonUpByString(const std::string& buttonName) const;

	// ----------------------- Axes ---------------------

	// Method to set controller's deadzone
	bool setDeadzoneInt(int deadzone);
	bool setDeadzoneFloat(float deadzone);

	// Method to be called when an axis is moved
	void setAxisState(SDL_GameControllerAxis axis, int value);

	// Method to get axis value normalized between -1 and 1 considering deadzone
	float getAxisValue(SDL_GameControllerAxis axis, bool useDeadzone = true) const;
	float getAxisValueByString(const std::string& axisName, bool useDeadzone = true) const;

	bool getAxisPastThreshold(SDL_GameControllerAxis axis, float threshold, bool useDeadzone = true) const;
	bool getAxisPastThresholdByString(const std::string& axisName, float threshold, bool useDeadzone = true) const;

	// --------------------- Touchpad --------------------

	// Method to be called when a touchpad finger is moved
	void setTouchpadFingerState(size_t touchpad, size_t finger, float x, float y, float pressure,
	                            inputState state);

	bool getTouchpadFinger(size_t touchpad, size_t finger) const;

	bool getTouchpadFingerDown(size_t touchpad, size_t finger) const;

	bool getTouchpadFingerUp(size_t touchpad, size_t finger) const;

	float getTouchpadFingerX(size_t touchpad, size_t finger) const;
	float getTouchpadFingerY(size_t touchpad, size_t finger) const;

	float getTouchpadFingerPressure(size_t touchpad, size_t finger) const;

	const TouchpadFinger* getTouchpadFingerState(size_t touchpad, size_t finger) const;
	bool isTouchpadValid(size_t touchpad) const;
	bool isTouchpadFingerValid(size_t touchpad, size_t finger) const;

	// --------------------- Utility ---------------------

	bool isConnected() const;
	static SDL_GameControllerButton stringToButton(const std::string& buttonName);
	static SDL_GameControllerAxis stringToAxis(const std::string& axisName);

private:
	std::unique_ptr<SDL_GameController, void(*)(SDL_GameController*)> controller;
	void printControllerInfo() const;

	// Buttons

	std::array<inputState, NUM_BUTTONS> buttonStates;
	std::vector<SDL_GameControllerButton> justDownButtons;
	std::vector<SDL_GameControllerButton> justUpButtons;

	// Axes

	int deadzone = 8000;

	// Stores the current state (value) of each axis
	// The value is raw from SDL, ranges from -32768 to 32767
	std::array<int, NUM_AXES> axisStates{};

	// Touchpad
	std::vector<std::vector<TouchpadFinger>> touchpadFingerStates;
};
#endif