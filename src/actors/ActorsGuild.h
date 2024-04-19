#ifndef ACTORSGUILD_H
#define ACTORSGUILD_H
#include <memory>

#include "../databases/ResourcesDB.h"
#include "../databases/SceneDB.h"
#include "../utils/Timer.h"
#include "Actor.h"
#include "ComponentManager.h"

class ActorsGuild
{
public:
    ~ActorsGuild() {
        members.clear();
        templates.clear();
    }

    ActorsGuild() = default;

    static ActorsGuild* getInstance() {
        return instance;
    }

    static void init(const ResourcesDB& configDB) {
        if (instance == nullptr) {
            instance = new ActorsGuild(configDB);
        }
    }

    static void update() {
        // update actors to add
        for (auto& actor : actorsToAdd) {
            members.push_back(std::move(actor));
        }
        actorsToAdd.clear();

        // start update for any new components
        for (auto& actor : members) {
            actor->start();
        }

        // add components to actors
        for (auto& actor : members) {
            actor->processAddedComponents();
        }

        // normal update
        for (auto& actor : members) {
            actor->update();
        }

        // late update
        for (auto& actor : members) {
            actor->lateUpdate();
        }

        // remove components
        for (auto& actor : members) {
            actor->processRemovedComponents();
        }

        // remove actors
        for (auto& actor : actorsToDestroy) {
            //actor->onDestroy();
            members.erase(std::remove_if(members.begin(), members.end(), [&actor](const std::shared_ptr<Actor>& a) { return a->actorId == actor->actorId; }), members.end());
        }
    }

    static void loadActors(const SceneDB& database) {
        Timer t;
        t.start();
        // Assuming the sceneDB has a "actors" array
        if (!database.mainDoc.doc.HasMember("actors") || !database.mainDoc.doc["actors"].IsArray()) {
            //std::cerr << "Scene file does not have an 'actors' array\n";
            return;
        }

        const rapidjson::Value& actorsArray = database.mainDoc.doc["actors"];
        members.reserve(actorsArray.Size());
        actorsToAdd.reserve(actorsArray.Size());
        std::string actorName;
        Datadoc actorDatadoc;
        for (rapidjson::SizeType i = 0; i < actorsArray.Size(); ++i) {
            // Convert actorData to a Datadoc
            actorDatadoc = Datadoc(actorsArray[i]);

            // Create actor
            actorsToAdd.push_back(std::make_shared<Actor>(nextActorId++, actorDatadoc, templates));

            // Add Components
            if (actorsArray[i].HasMember("components") && actorsArray[i]["components"].IsObject()) {
                loadComponentsOnActor(actorsArray[i]["components"], actorsToAdd.back()->justAddedComponents);
            }

            // Inject Actor References
            for (const auto& component : actorsToAdd.back()->justAddedComponents) {
                actorsToAdd.back()->injectActorReferences(component.second);
            }
        }
        t.stop();
        //std::cerr << "finished loading actors\n" << t;
    }

    static void loadComponentsOnActor(const rapidjson::Value& c, std::map<std::string, std::shared_ptr<Component>>& components) {
        for (auto iter = c.MemberBegin(); iter != c.MemberEnd(); iter++) {
            std::string componentName = iter->name.GetString();
            // Check for Existing component, likely inherited from a template
            if (components.find(componentName) == components.end()) {
                std::string componentType = iter->value["type"].GetString();
                components[componentName] = std::make_shared<Component>(componentName, componentType);
            }

            for (auto propertyIter = iter->value.MemberBegin(); propertyIter != iter->value.MemberEnd(); propertyIter++) {
                std::string propertyName = propertyIter->name.GetString();
                if (propertyIter->value.IsString()) {
                    components[componentName]->addStringProperty(propertyName, propertyIter->value.GetString());
                }
                else if (propertyIter->value.IsInt()) {
                    components[componentName]->addIntProperty(propertyName, propertyIter->value.GetInt());
                }
                else if (propertyIter->value.IsFloat()) {
                    components[componentName]->addFloatProperty(propertyName, propertyIter->value.GetFloat());
                }
                else if (propertyIter->value.IsBool()) {
                    components[componentName]->addBoolProperty(propertyName, propertyIter->value.GetBool());
                }
            }
        }
    }  

    static std::shared_ptr<Actor>& getActorById(const size_t key) {
        return members[key - templates.size()];
    }

    static luabridge::LuaRef getActorByName(const std::string& name) {
        luabridge::LuaRef returnValue = luabridge::LuaRef(ComponentManager::luaState);
        bool invalidActor = false;

        for (const auto& actor : actorsToAdd) {
            if (actor->getName() == name) {
                for (const auto& toBeDestroyedActor : actorsToDestroy) {
                    invalidActor = actor->actorId == toBeDestroyedActor->actorId;
                }
                if (!invalidActor) {
                    returnValue = actorToLuaRef(actor);
                    break;
                }
            }
        }
        for (const auto& actor : members) {
            if (actor->getName() == name) {
                for (const auto& toBeDestroyedActor : actorsToDestroy) {
                    invalidActor = actor->actorId == toBeDestroyedActor->actorId;
                }
                if (!invalidActor) {
                    returnValue = actorToLuaRef(actor);
                    break;
                }
            }
        }
        return returnValue;
    }

    static luabridge::LuaRef getActorsByName(const std::string& name) {
        luabridge::LuaRef actorsTable = luabridge::newTable(ComponentManager::luaState);
        bool invalidActor = false;
        int i = 1;

        for (const auto& actor : actorsToAdd) {
            if (actor->getName() == name) {
                for (const auto& toBeDestroyedActor : actorsToDestroy) {
                    invalidActor = actor->actorId == toBeDestroyedActor->actorId;
                }
                if (!invalidActor) {
                    actorsTable[i++] = actorToLuaRef(actor);
                }
            }
        }
        for (const auto& actor : members) {
            if (actor->getName() == name) {
                for (const auto& toBeDestroyedActor : actorsToDestroy) {
                    invalidActor = actor->actorId == toBeDestroyedActor->actorId;
                }
                if (!invalidActor) {
                    actorsTable[i++] = actorToLuaRef(actor);
                }
            }
        }
        return actorsTable;
    }

    static luabridge::LuaRef instantiateActorFromTemplate(const std::string& templateName) {
        if (auto it = templates.find(templateName); it != templates.end()) {
            actorsToAdd.push_back(std::make_shared<Actor>(nextActorId++));
            actorsToAdd.back()->updateActor(it->second);
            for (const auto& component : actorsToAdd.back()->justAddedComponents) {
                actorsToAdd.back()->injectActorReferences(component.second);
            }
            return actorToLuaRef(actorsToAdd.back());
        }
        else {
            //std::cerr << "error: template " << templateName << " is missing";
            exit(0);
        }
    }

    static void destroyActor(Actor& actor) {
        for (auto& component : actor.justAddedComponents) {
            actor.removeComponent(component.second->component);
        }
        for (auto& component : actor.components) {
            actor.removeComponent(component.second->component);
        }
        actorsToDestroy.push_back(std::make_shared<Actor>(actor));
    }

    static luabridge::LuaRef actorToLuaRef(std::shared_ptr<Actor> actor) {
        // Push the actor onto the Lua stack
        luabridge::push(ComponentManager::luaState, actor.get());

        // Create LuaRef from the object on the top of the stack
        luabridge::LuaRef ref = luabridge::LuaRef::fromStack(ComponentManager::luaState, -1);

        // Now pop the actor back off the stack since LuaRef has a copy
        lua_pop(ComponentManager::luaState, 1);

        return ref;
    }

    static void clear() {
        for (auto& actor : actorsToAdd) {
            members.push_back(std::move(actor));
        }
        actorsToAdd.clear();

        for (auto& actor : members) {
            if (!actor->dontDestroyOnLoad) {
                destroyActor(*actor);
            }
        }

        for (auto& actor : actorsToDestroy) {
            //actor->onDestroy();
            members.erase(std::remove_if(members.begin(), members.end(), [&actor](const std::shared_ptr<Actor>& a) { return a->actorId == actor->actorId; }), members.end());
        }
        actorsToDestroy.clear();
    }

    static inline std::vector<std::shared_ptr<Actor>> members = {};
    static inline std::vector<std::shared_ptr<Actor>> actorsToAdd = {};
    static inline std::vector<std::shared_ptr<Actor>> actorsToDestroy = {};
private:
    static inline ActorsGuild* instance = nullptr;
    static inline int nextActorId = 0;
    static inline std::map<std::string, std::shared_ptr<Actor>> templates = {};

    ActorsGuild(const ResourcesDB& configDB) {
        instance = this;

        // Register Actor namespace with lua
        luabridge::getGlobalNamespace(ComponentManager::luaState)
            .beginNamespace("Actor")
            .addFunction("Find", &ActorsGuild::getActorByName)
            .addFunction("FindAll", &ActorsGuild::getActorsByName)
            .addFunction("Instantiate", &ActorsGuild::instantiateActorFromTemplate)
            .addFunction("Destroy", &ActorsGuild::destroyActor)
            .endNamespace();

        // Register Actor class with Lua
        luabridge::getGlobalNamespace(ComponentManager::luaState)
            .beginClass<Actor>("actor")
            .addFunction("GetName", &Actor::getName)
            .addFunction("GetID", &Actor::getID)
            .addFunction("GetComponentByKey", &Actor::getComponentByKey)
            .addFunction("GetComponent", &Actor::getComponentByType)
            .addFunction("GetComponents", &Actor::getComponentsByType)
            .addFunction("AddComponent", &Actor::addComponent)
            .addFunction("RemoveComponent", &Actor::removeComponent)
            .endClass();

        // timer for debugging optimization
        Timer t;

        // load templates
        t.start();
        for (const auto& temp : configDB.templates) {
            // Create actor
            templates[temp.first] = std::make_shared<Actor>(nextActorId++, temp.second);

            // Add Components
            if (temp.second.doc.HasMember("components") && temp.second.doc["components"].IsObject()) {
                loadComponentsOnActor(temp.second.doc["components"], templates[temp.first]->justAddedComponents);
            }
        }

        t.stop();
        //std::cerr << "finished loading templates\n" << t;
    }
};
#endif
