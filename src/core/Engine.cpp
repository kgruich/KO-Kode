#include "Engine.h"
#include <glm/geometric.hpp>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

Engine::Engine() {
	instance = this;
	resourcesDB.searchResourcesFolder();
	resourcesDB.loadData();
	resourcesDB.searchInitialScene();

	componentManager.init();
	audioDB.init();
	Input::init();
	actorsGuild.init(resourcesDB);
	renderer->init(resourcesDB);

	luabridge::getGlobalNamespace(componentManager.luaState)
        .beginNamespace("Scene")
        .addFunction("Load", &Engine::setSceneToLoad)
		.addFunction("GetCurrent", &Engine::getCurrentSceneName)
		.addFunction("DontDestroy", &Engine::markActorDontDestroyOnLoad)
		.endNamespace();


	sceneToLoad = resourcesDB.initialSceneName;
	loadScene();
}

void Engine::gameLoop()
{
	while (running)
	{
		loadScene();

		input();

		update();

		render();

		lateUpdate();
	}
	shutDown();
}

void Engine::setSceneToLoad(const std::string& sceneName) {
	sceneToLoad = sceneName;
}

void Engine::markActorDontDestroyOnLoad(Actor& a) {
	int id = a.getID();
	ActorsGuild::getActorById(id)->dontDestroyOnLoad = true;
}

void Engine::loadScene() {
	if (sceneToLoad.empty()) {
        return;
    }

	currentSceneName = sceneToLoad;
	currentScene.release();
	ActorsGuild::clear();
	currentScene = std::make_unique<Scene>(resourcesDB, sceneToLoad);
	sceneToLoad = "";
}

void Engine::input()
{
	if (!running) { return; }

	if (Helper::GetFrameNumber() == 151) {
		running = true;
	}

	SDL_Event e;
	while (Helper::SDL_PollEvent498(&e)) {
		Input::processEvent(e);
		if (e.type == SDL_QUIT) {
			running = false;
		}
	}
}

void Engine::update()
{
	ActorsGuild::update();
}

void Engine::render()
{
	//std::cerr << "Frame " << Helper::GetFrameNumber() << " done\n";
	renderer->render();
	return;
}

void Engine::lateUpdate() {
	Input::lateUpdate();
}

void Engine::shutDown() {
	//TODO: shut down all singletons + scene
	Renderer::shutDown();
    SDL_Quit();
}
