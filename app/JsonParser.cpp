#include "JsonParser.h"
#include <cstdio>

JsonParser::JsonParser(const std::string& json) : _json(json) {}

JsonParser::~JsonParser() {}

void JsonParser::trim(std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\"");
    size_t end = str.find_last_not_of(" \t\n\r\",");
    if (start != std::string::npos && end != std::string::npos) {
        str = str.substr(start, end - start + 1);
    }
}

std::string JsonParser::extractValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) {
        return "";
    }
    
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    size_t valueStart = colonPos + 1;
    size_t valueEnd = json.find_first_of(",}", valueStart);
    if (valueEnd == std::string::npos) {
        return "";
    }
    
    std::string value = json.substr(valueStart, valueEnd - valueStart);
    trim(value);
    return value;
}

bool JsonParser::parse() {
    // data 객체 찾기
    size_t dataPos = _json.find("\"data\"");
    if (dataPos == std::string::npos) {
        return false;
    }
    
    size_t dataStart = _json.find("{", dataPos);
    if (dataStart == std::string::npos) {
        return false;
    }
    
    size_t dataEnd = _json.find("}", dataStart);
    if (dataEnd == std::string::npos) {
        return false;
    }
    
    std::string dataJson = _json.substr(dataStart, dataEnd - dataStart + 1);
    
    // 각 필드 추출
    const char* keys[] = {"id", "ip", "run", "arm1", "arm2", "stan", "body", 
                          "leg1", "leg2", "le1l", "le2l", "ank1", "ank2"};
    
    for (int i = 0; i < 13; i++) {
        std::string value = extractValue(dataJson, keys[i]);
        if (!value.empty()) {
            _data[keys[i]] = value;
        }
    }
    
    return !_data.empty();
}

bool JsonParser::hasKey(const std::string& key) const {
    return _data.find(key) != _data.end();
}

std::string JsonParser::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = _data.find(key);
    if (it != _data.end()) {
        return it->second;
    }
    return defaultValue;
}

int JsonParser::getInt(const std::string& key, int defaultValue) const {
    auto it = _data.find(key);
    if (it != _data.end()) {
        return std::stoi(it->second);
    }
    return defaultValue;
}
