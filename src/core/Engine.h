#ifndef ENGINE_H
#define ENGINE_H

#include "Scene.h"
#include "../input/Input.h"
#include "../rendering/Renderer.h"
#include "../databases/AudioDB.h"
#include "../actors/ComponentManager.h"

class Engine
{
public:
	Engine();

	static void gameLoop();

	static void setSceneToLoad(const std::string& sceneName);

	static std::string getCurrentSceneName() { return currentSceneName; }

	static void markActorDontDestroyOnLoad(Actor& a);

private:
	static void loadScene();

	static void input();
	static void update();
	static void render();
	static void lateUpdate();

	static void shutDown();

	static inline Engine* instance = nullptr;

	static inline bool running = true;
	static inline std::ostringstream out;

	static inline Renderer* renderer;
	static inline ComponentManager componentManager;
	static inline ActorsGuild actorsGuild;
	static inline ResourcesDB resourcesDB;
	static inline AudioDB audioDB;

	static inline std::unique_ptr<Scene> currentScene = nullptr;
	static inline std::string currentSceneName = "";
	static inline std::string sceneToLoad = "";
};

#endif
