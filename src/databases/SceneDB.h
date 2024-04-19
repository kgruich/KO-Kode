#ifndef SCENEDB_H
#define SCENEDB_H
#include "BaseDB.h"

class SceneDB : public BaseDB {
public:
    SceneDB() : BaseDB() {}

    // for filename -> filepath
    explicit SceneDB(const std::string& sceneName) : BaseDB(fs::current_path() / "resources" / "scenes" /  (sceneName + ".scene")) {
        name = "SceneDB";
    }

    void loadData() override {
        if (!fs::exists(dataPath)) {
            std::cout << "error: scene " + dataPath.stem().string() + " is missing";
            exit(0);
        }
        // Get Scene Data
        readJsonFile(dataPath.string(), mainDoc.doc);
    }
};
#endif