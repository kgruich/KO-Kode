#include "ComponentManager.h"

#include <glm/vec2.hpp>

Component::Component() : name(""), type(""), component(luabridge::newTable(ComponentManager::luaState)) {
    addDefaultProperties();
}

Component::Component(const std::string& name, const std::string& type)
    : name(name), type(type), component(ComponentManager::getInstance()->getComponentInstance(type)) {
    addDefaultProperties();
}

Component::Component(Component& other) : name(other.name), type(other.type), component(luabridge::newTable(ComponentManager::luaState))
{
    ComponentManager::getInstance()->establishInstance(component, other.component);
}

void Component::addDefaultProperties()
{
    addStringProperty("key", name);
    addBoolProperty("enabled", true);
}

void Component::addStringProperty(const std::string& key, const std::string& value)
{
    component[key] = value;
}

void Component::addIntProperty(const std::string& key, const int value)
{
    component[key] = value;
}

void Component::addFloatProperty(const std::string& key, const float value)
{
    component[key] = value;
}

void Component::addBoolProperty(const std::string& key, const bool value)
{
    component[key] = value;
}

ComponentManager* ComponentManager::getInstance() {
    if (instance == nullptr) {
        instance = new ComponentManager();
    }
    return instance;
}

void ComponentManager::initState() {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
}

void ComponentManager::initFunctions() {
    // Debug API functions
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Debug")
        .addFunction("Log", &print)
        .addFunction("LogError", &printError)
        .endNamespace();

    // Application API functions
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Application")
        .addFunction("Quit", &quit)
        .addFunction("Sleep", &sleep)
        .addFunction("GetFrame", &Helper::GetFrameNumber)
        .addFunction("OpenURL", &openURL)
        .endNamespace();

    // External classes API
    luabridge::getGlobalNamespace(luaState)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();
}

void ComponentManager::initComponents() {
    if (!std::filesystem::exists(componentPath)) {
        //std::cerr << "error: component path does not exist\n";
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(componentPath)) {
        if (const auto statusCode = luaL_dofile(luaState, entry.path().string().c_str()); statusCode != LUA_OK) {
            std::cout << "problem with lua file " << entry.path().stem().string();
            //std::cerr << "error: " << statusCode;
            exit(0);
        }

        std::string componentName = entry.path().stem().string();
        originalComponents.insert({ componentName, std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(luaState, componentName.c_str())) });
    }
}

void ComponentManager::print(const std::string s) {
    std::cout << s << '\n';
}

void ComponentManager::printError(const std::string s) {
    std::cerr << s << '\n';
}

void ComponentManager::quit() {
    exit(0);
}

void ComponentManager::sleep(int durationMs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
}

void ComponentManager::openURL(const std::string& url) {
    #if defined(_WIN32)
        std::string command = "start " + url;
        system(command.c_str());
    #elif defined(__APPLE__)
        std::string command = "open " + url;
        system(command.c_str());
    #else
        std::string command = "xdg-open " + url;
        system(command.c_str());
    #endif
}

void ComponentManager::establishInstance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& sourceTable) {
    // Create a new metatable to establish inheritance 
    luabridge::LuaRef newMetatable = luabridge::newTable(luaState);
    newMetatable["__index"] = sourceTable;

    // Use raw lua C-API (lua stack) to preform the metatable assignment
    instanceTable.push(luaState);
    newMetatable.push(luaState);
    lua_setmetatable(luaState, -2);
    lua_pop(luaState, 1);
}

luabridge::LuaRef ComponentManager::getComponentInstance(const std::string& componentName) {
    const auto it = originalComponents.find(componentName);
    if (it == originalComponents.end()) {
        std::cout << "error: failed to locate component " << componentName;
        exit(0);
    }
    luabridge::LuaRef instanceTable = luabridge::newTable(luaState);
    establishInstance(instanceTable, *it->second);
    return instanceTable;
}
