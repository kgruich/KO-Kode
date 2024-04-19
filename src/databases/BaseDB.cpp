#include "BaseDB.h"

#include <iostream>
#include <rapidjson/filereadstream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "../utils/Timer.h"

static void merge(rapidjson::Value& dest, const rapidjson::Value& src, rapidjson::Document::AllocatorType& allocator) {
    if (dest.IsNull()) {
        dest.CopyFrom(src, allocator);
        return;
    }
    for (auto it = src.MemberBegin(); it != src.MemberEnd(); ++it) {
        if (dest.HasMember(it->name)) {
            if (dest[it->name].IsObject() && it->value.IsObject()) {
                merge(dest[it->name], it->value, allocator);
            }
            else {
                dest[it->name].CopyFrom(it->value, allocator);
            }
        }
        else {
            rapidjson::Value name;
            name.CopyFrom(it->name, allocator);
            rapidjson::Value value;
            value.CopyFrom(it->value, allocator);
            dest.AddMember(name.Move(), value.Move(), allocator);
        }
    }
}

void Datadoc::printDocument(const rapidjson::Document& doc, std::ostream& os) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    os << buffer.GetString() << '\n';
}

void Datadoc::addJsonFile(rapidjson::Document& addedDoc) {
    //std::cerr << "Before merge:\n";
    //printDocument(this->doc, std::cerr);
    //printDocument(addedDoc, std::cerr);
    merge(this->doc, addedDoc, this->doc.GetAllocator());
    //std::cerr << "After merge:\n";
    //printDocument(this->doc, std::cerr);
}

std::vector<std::string> Datadoc::getStringVector(const std::string_view key, const std::vector<std::string>& fallback) const {
    return getOptionalStringVector(key).value_or(fallback);
}

std::string Datadoc::getString(const std::string_view key, const std::string& fallback) const {
    return getOptionalString(key).value_or(fallback);
}

std::unique_ptr<char[]> Datadoc::getCharPointer(const std::string_view key, const char* fallback) const {
    auto result = getOptionalCharPointer(key);
    if (result.has_value()) {
        return std::move(result.value());
    }
    else {
        std::unique_ptr<char[]> cstr(new char[std::strlen(fallback) + 1]);
        strncpy(cstr.get(), fallback, std::strlen(fallback) + 1);
        return cstr;
    }
}

double Datadoc::getDouble(const std::string_view key, const double fallback) const
{
    return getOptionalDouble(key).value_or(fallback);
}

int Datadoc::getInt(const std::string_view key, const int fallback) const {
    return getOptionalInt(key).value_or(fallback);
}

float Datadoc::getFloat(const std::string_view key, const float fallback) const {
    return getOptionalFloat(key).value_or(fallback);
}

char Datadoc::getChar(const std::string_view key, const char fallback) const {
    return getOptionalChar(key).value_or(fallback);
}

bool Datadoc::getBool(const std::string_view key, const bool fallback) const {
    return getOptionalBool(key).value_or(fallback);
}

std::optional<std::vector<std::string>> Datadoc::getOptionalStringVector(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsArray()) {
        std::vector<std::string> result;
        for (const auto& value : it->value.GetArray()) {
            if (value.IsString()) {
                result.emplace_back(value.GetString());
            }
            else {
                return std::nullopt;
            }
        }
        return result;
    }
    return std::nullopt;
}   

std::optional<std::string> Datadoc::getOptionalString(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsString()) {
        return it->value.GetString();
    }
    return std::nullopt;
}

std::optional<std::unique_ptr<char[]>> Datadoc::getOptionalCharPointer(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsString()) {
        std::string str = it->value.GetString();
        std::unique_ptr<char[]> cstr(new char[str.length() + 1]);
        strncpy(cstr.get(), str.c_str(), str.length() + 1);
        return cstr;
    }
    return std::nullopt;
}

std::optional<int> Datadoc::getOptionalInt(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsNumber()) {
        return it->value.GetInt();
    }
    return std::nullopt;
}

std::optional<double> Datadoc::getOptionalDouble(const std::string_view key) const
{
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsNumber()) {
        return it->value.GetDouble();
    }
    return std::nullopt;
}

std::optional<float> Datadoc::getOptionalFloat(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsNumber()) {
        return it->value.GetFloat();
    }
    return std::nullopt;
}

std::optional<char> Datadoc::getOptionalChar(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsString() && it->value.GetStringLength() == 1) {
        return it->value.GetString()[0];
    }
    return std::nullopt;
}

std::optional<bool> Datadoc::getOptionalBool(const std::string_view key) const {
    if (const auto it = doc.FindMember(key.data()); it != doc.MemberEnd() && it->value.IsBool()) {
        return it->value.GetBool();
    }
    return std::nullopt;
}

void BaseDB::loadData() {
    rapidjson::Document tempdoc;
    for (const auto& file : fs::directory_iterator(dataPath)) {
        if (file.path().extension() == ".config" || file.path().extension() == ".scene") {
            readJsonFile(file.path().string(), tempdoc);
            if (!readJsonFile(file.path().string(), tempdoc)) {
                //std::cerr << "Failed to read JSON file: " << file.path().string() << '\n';
                continue;
            }
            mainDoc.addJsonFile(tempdoc);
            tempdoc.Clear();
        }
    }
}

bool BaseDB::readJsonFile(const std::string& path, rapidjson::Document& outDocument) {
    Timer t;
    t.start();
    FILE* file_pointer = nullptr;
    #ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
    #else
		file_pointer = fopen(path.c_str(), "rb");
    #endif
    if (file_pointer == nullptr) {
        std::cerr << "Failed to open file: " << path << '\n';
        return false;
    }
    char buffer[65536]{};
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    outDocument.ParseStream(stream);
    std::fclose(file_pointer);

    if (outDocument.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = outDocument.GetParseError();
        std::cout << "error parsing json at [" << path << "]\n";
        exit(0);
    }
    t.stop();
    //std::cerr << "finished reading json from " << path << "\n" << t;
    return true;
}