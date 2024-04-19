#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <memory>

#include "../actors/Actor.h"
#include "../actors/ActorsGuild.h"

class SceneDB;

class Scene {
public:
	Scene() = default;

	explicit Scene(const ResourcesDB& configDB, const std::string& sceneName) : sceneDB(sceneName), sceneName(sceneName) {
		sceneDB.loadData();
		ActorsGuild::loadActors(sceneDB);
	}

	std::string getSceneName() const { return sceneName; }

private:
	std::string sceneName;
	SceneDB sceneDB;
};

#endif