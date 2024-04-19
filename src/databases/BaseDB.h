#ifndef BASEDB_H
#define BASEDB_H
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

#include "rapidjson/document.h"

namespace fs = std::filesystem;

class Datadoc {
public:
    rapidjson::Document doc;
    Datadoc() = default;

    Datadoc(rapidjson::Document& d) : doc(std::move(d)) {}

    Datadoc(const rapidjson::Value& v) {
        if (!v.IsObject()) {
            //std::cerr << "Actor data is not an object\n";
            exit(1);
        }
        doc.CopyFrom(v, doc.GetAllocator());
    }

    void replaceDoc(rapidjson::Document& d) {
        doc = std::move(d);
    }

    void addJsonFile(rapidjson::Document& addedDoc);
    static void printDocument(const rapidjson::Document& doc, std::ostream& os);

    // getValue Variants
    std::vector<std::string> getStringVector(const std::string_view key, const std::vector<std::string>& fallback) const;
    std::string getString(const std::string_view key, const std::string& fallback) const;
    std::unique_ptr<char[]> getCharPointer(const std::string_view key, const char* fallback) const;
    int getInt(const std::string_view key, const int fallback) const;
    double getDouble(const std::string_view key, const double fallback) const;
    float getFloat(const std::string_view key, const float fallback) const;
    char getChar(const std::string_view key, const char fallback) const;
    bool getBool(const std::string_view key, const bool fallback) const;

    // getOptionalValue Variants
    std::optional<std::vector<std::string>> getOptionalStringVector(const std::string_view key) const;
    std::optional<std::string> getOptionalString(const std::string_view key) const;
    std::optional<std::unique_ptr<char[]>> getOptionalCharPointer(const std::string_view key) const;
    std::optional<int> getOptionalInt(const std::string_view key) const;
    std::optional<double> getOptionalDouble(const std::string_view key) const;
    std::optional<float> getOptionalFloat(const std::string_view key) const;
    std::optional<char> getOptionalChar(const std::string_view key) const;
    std::optional<bool> getOptionalBool(const std::string_view key) const;
};

class BaseDB {
public:
    virtual ~BaseDB() {
        //std::cerr << "BaseDB destructor called for " << name << '\n' << "input operation timer:\n";
    }

    virtual void loadData();

    BaseDB() = default;

    explicit BaseDB(fs::path path) : dataPath(std::move(path)) {}

    Datadoc mainDoc;
protected:
    std::string name;
	fs::path dataPath;

    static bool readJsonFile(const std::string& path, rapidjson::Document& outDocument);
};
#endif
