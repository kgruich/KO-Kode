#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include <string>
#include <filesystem>
#include <thread>
#include <chrono>
#include "../../external_helpers/Helper.h"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component
{
public:
    std::string name;
    std::string type;
    luabridge::LuaRef component;

    // Default ctor
    Component();

    // Ctor with name and type
    Component(const std::string& name, const std::string& type);

    // Copy ctor, generally used for copying from templates
    // Can't be const because of LuaRef inheritance??
    Component(Component& other);

    void addDefaultProperties();

    void addStringProperty(const std::string& key, const std::string& value);
    void addIntProperty(const std::string& key, const int value);
    void addFloatProperty(const std::string& key, const float value);
    void addBoolProperty(const std::string& key, const bool value);
};

class ComponentManager
{
public:
    static ComponentManager* getInstance();

    void init() {
        instance = this;
        initState();
        initFunctions();
        initComponents();
    }

    static void initState();

    static void initFunctions();

    static void initComponents();

    static void establishInstance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& sourceTable);

    static luabridge::LuaRef getComponentInstance(const std::string& componentName);

    static inline ComponentManager* instance = nullptr;
    static inline lua_State* luaState = nullptr;
    static inline int addComponentCount = 0;

private:
    static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> originalComponents = {};
    static inline std::string componentPath = "resources/component_types/";

    static void print(const std::string s);
    static void printError(const std::string s);
    static void quit();
    static void sleep(int durationMs);
    static void openURL(const std::string& url);
};
#endif