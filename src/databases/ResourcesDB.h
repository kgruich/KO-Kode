#ifndef RESOURCESDB_H
#define RESOURCESDB_H
#include <filesystem>
#include <iostream>
#include <map>
#include "../utils/Timer.h"
#include "BaseDB.h"

namespace fs = std::filesystem;

class ResourcesDB : public BaseDB
{
public:
    std::string initialSceneName;
    fs::path templatePath = dataPath / "actor_templates";
    std::map<std::string, Datadoc> templates;

    explicit ResourcesDB()
        : BaseDB(fs::current_path() / "resources") {
        name = "ResourceDB";
    }

    void loadData() override {
        Timer t;
        // Get Basic Config/Rendering Data
        t.start();
        for (const auto& file : fs::directory_iterator(dataPath)) {
            if (file.path().extension() == ".config") {
                rapidjson::Document tempdoc;
                readJsonFile(file.path().string(), tempdoc);
                if (!readJsonFile(file.path().string(), tempdoc)) {
                    //std::cerr << "Failed to read JSON file: " << file.path().string() << '\n';
                    continue;
                }
                mainDoc.addJsonFile(tempdoc);
            }
        }
        t.stop();
        //std::cerr << "finished parsing config data\n" << t;
        //Datadoc::printDocument(mainDoc.doc, std::cerr);
        // Get Template Data
        if (!fs::exists(templatePath))
            return;
        t.start();
        std::vector<std::string> templateNames;
        for (const auto& file : fs::directory_iterator(templatePath)) {
            if (file.path().extension() == ".template") {
                std::string templateName = file.path().stem().string();
                Datadoc templateDoc;
                readJsonFile(file.path().string(), templateDoc.doc);
                //std::cerr << "Template: " << templateName << '\n';
                //Datadoc::printDocument(templateDoc.doc, std::cerr);
                templates.emplace(std::move(templateName), std::move(templateDoc));
            }
        }
        t.stop();
        //std::cerr << "finished parsing template data\n" << t;
    }

    static void searchResourcesFolder() {
        if (!fs::exists(std::string("resources")))
        {
            //std::cerr << "error: " << "/resources" << " missing.\n";
            //std::cerr << "Current path is: " << fs::current_path() << "\n";
            std::cout << "error: resources/ missing";
            exit(0);
        }
        else if (!fs::exists(std::string("resources/game.config")))
        {
            std::cout << "error: resources/game.config missing";
            exit(0);
        }
    }

    void searchInitialScene() {
        if (const auto name = mainDoc.getOptionalString("initial_scene"); !name.has_value())
        {
            std::cout << "error: initial_scene unspecified";
            exit(0);
        }
        else {
            initialSceneName = name.value();
        }
    }
};

#endif