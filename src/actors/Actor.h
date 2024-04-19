#ifndef ACTOR_H
#define ACTOR_H
#include <string>
#include <map>
#include "../databases/BaseDB.h"
#include "ComponentManager.h"

#include <map>

class Actor {
public:
    int actorId = -1;
    std::string actorName;
    bool dontDestroyOnLoad = false;

    std::map<std::string, std::shared_ptr<Component>> justAddedComponents;
    std::map<std::string, std::shared_ptr<Component>> components;
    std::vector<std::string> componentsToRemove;

    // Default, something is wrong if this gets used
    Actor() = default;

    // ID only, used before checking for template and specific data
    explicit Actor(const int actorId) : actorId(actorId) {}

    //Datadoc constructor, no template
    Actor(const int actorId, const Datadoc& actorDatadoc) {
        this->actorId = actorId;
        updateActor(actorDatadoc);
    }

    //Datadoc constructor that checks for a template
    Actor(const int actorId, const Datadoc& actorDatadoc, const std::map<std::string, std::shared_ptr<Actor>>& templates) {
        this->actorId = actorId;
        const std::string templateName = actorDatadoc.getString("template", "");
        if (!templateName.empty()) {
            if (auto it = templates.find(templateName); it != templates.end()) {
                updateActor(it->second);
            }
            else {
                std::cout << "error: template " << templateName << " is missing";
                exit(0);
            }
        }
        updateActor(actorDatadoc);
    }

    // uses a Datadoc to update fields, defaults to current values
    void updateActor(const Datadoc& doc) {
        // Name
        actorName = doc.getString(name_suffix, actorName);
    }

    // uses a template or other Actor
    void updateActor(const std::shared_ptr<Actor>& other) {
        actorName = other->actorName;
        for (const auto& component : other->justAddedComponents) {
            justAddedComponents[component.first] = std::make_shared<Component>(*component.second);
        }
        for (const auto& component : other->components) {
            components[component.first] = std::make_shared<Component>(*component.second);
        }
        for (const auto& component : other->componentsToRemove) {
            componentsToRemove.push_back(component);
        }
    }

    std::string getName() const {
        return actorName;
    }

    int getID() const {
        return actorId;
    }

    luabridge::LuaRef getComponentByKey(const std::string& componentName) {
        luabridge::LuaRef returnValue = luabridge::LuaRef(ComponentManager::luaState);

        // Return nil if component is to be removed
        for (const auto& toBeRemovedComponentName : componentsToRemove) {
            if (componentName == toBeRemovedComponentName)
                return returnValue;
        }

        if (auto it = justAddedComponents.find(componentName); it != justAddedComponents.end()) {
            returnValue = it->second->component;
        }
        else if (auto it = components.find(componentName); it != components.end()) {
            returnValue = it->second->component;
        }
        return returnValue;
    }

    luabridge::LuaRef getComponentByType(const std::string& componentType) {
        luabridge::LuaRef returnValue = luabridge::LuaRef(ComponentManager::luaState);
        bool invalidComponent = false;

        for (const auto& component : justAddedComponents) {
            if (component.second->type == componentType) {
                // Return nil if component is to be removed
                for (const auto& toBeRemovedComponentName : componentsToRemove) {
                    invalidComponent = component.second->name == toBeRemovedComponentName;
                }
                if (!invalidComponent) {
                    returnValue = component.second->component;
                    break;
                }
            }
        }

        for (const auto& component : components) {
            if (component.second->type == componentType) {
                // Return nil if component is to be removed
                for (const auto& toBeRemovedComponentName : componentsToRemove) {
                    invalidComponent = component.second->name == toBeRemovedComponentName;
                }
                if (!invalidComponent) {
                    returnValue = component.second->component;
                    break;
                }
            }
        }

        return returnValue;
    }

    luabridge::LuaRef getComponentsByType(const std::string& componentType) {
        luabridge::LuaRef componentsTable = luabridge::newTable(ComponentManager::luaState);
        bool invalidComponent = false;
        int i = 1;

        for (const auto& component : justAddedComponents) {
            if (component.second->type == componentType) {
                // Return nil if component is to be removed
                for (const auto& toBeRemovedComponentName : componentsToRemove) {
                    invalidComponent = component.second->name == toBeRemovedComponentName;
                }
                if (!invalidComponent) {
                    componentsTable[i++] = component.second->component;
                }
            }
        }
        for (const auto& component : components) {
            if (component.second->type == componentType) {
                // Return nil if component is to be removed
                for (const auto& toBeRemovedComponentName : componentsToRemove) {
                    invalidComponent = component.second->name == toBeRemovedComponentName;
                }
                if (!invalidComponent) {
                    componentsTable[i++] = component.second->component;
                }
            }
        }
        return componentsTable;
    }

    luabridge::LuaRef addComponent(const std::string& componentType) {
        std::string componentName = "r" + std::to_string(ComponentManager::addComponentCount);
        justAddedComponents[componentName] = std::make_shared<Component>(componentName, componentType);
        injectActorReferences(justAddedComponents[componentName]);
        ComponentManager::addComponentCount++;

        return justAddedComponents[componentName]->component;
    }

    void removeComponent(const luabridge::LuaRef& component) {
        component["enabled"] = false;
        componentsToRemove.push_back(component["key"]);
    }

    void injectActorReferences(std::shared_ptr<Component> component) {
        (component->component)["actor"] = this;
    }

    void start() {
        for (const auto& component : justAddedComponents) {
            luabridge::LuaRef OnStart = component.second->component["OnStart"];
            luabridge::LuaRef enabled = component.second->component["enabled"];

            try {
                if (enabled.cast<bool>() && OnStart.isFunction()) {
                    OnStart(component.second->component);
                }
            }
            catch (const luabridge::LuaException& e) {
                reportError(e);
            }
        }
    }

    void processAddedComponents() {
        for (const auto& component : justAddedComponents) {
            components[component.first] = component.second;
        }
        justAddedComponents.clear();
    }

    void update() {
        for (const auto& component : components) {
            luabridge::LuaRef OnUpdate = component.second->component["OnUpdate"];
            luabridge::LuaRef enabled = component.second->component["enabled"];

            try {
                if (enabled.cast<bool>() && OnUpdate.isFunction()) {
                    OnUpdate(component.second->component);
                }
            }
            catch (const luabridge::LuaException& e) {
                reportError(e);
            }
        }
    }

    void lateUpdate() {
        for (const auto& component : components) {
            luabridge::LuaRef OnLateUpdate = component.second->component["OnLateUpdate"];
            luabridge::LuaRef enabled = component.second->component["enabled"];

            try {
                if (enabled.cast<bool>() && OnLateUpdate.isFunction()) {
                    OnLateUpdate(component.second->component);
                }
            }
            catch (const luabridge::LuaException& e) {
                reportError(e);
            }
        }
    }

    void processRemovedComponents() {
        for (const auto& component : componentsToRemove) {
            components.erase(component);
        }
        componentsToRemove.clear();
    }

    void onDestroy() {
        for (const auto& component : components) {
            luabridge::LuaRef onDestroy = component.second->component["OnDestroy"];
            luabridge::LuaRef enabled = component.second->component["enabled"];

            try {
                if (enabled.cast<bool>() && onDestroy.isFunction()) {
                    onDestroy(component.second->component);
                }
            }
            catch (const luabridge::LuaException& e) {
                reportError(e);
            }
        }
    }

    void reportError(const luabridge::LuaException& e) {
        std::string error_message = e.what();

        // Normalize file paths across platforms
        std::replace(error_message.begin(), error_message.end(), '\\', '/');

        // Display error with color codes
        std::cout << "\033[31m" << actorName << " : " << error_message << "\033[0m" << std::endl;
    }

    bool operator==(const Actor& other) const {
        return actorId == other.actorId;
    }

private:
    static constexpr const char* name_suffix = "name";
};

#endif
