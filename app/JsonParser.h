#ifndef JSON_PARSER_H_
#define JSON_PARSER_H_

#include <string>
#include <map>

class JsonParser {
public:
    JsonParser(const std::string& json);
    ~JsonParser();
    
    bool parse();
    bool hasKey(const std::string& key) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    
private:
    std::string _json;
    std::map<std::string, std::string> _data;
    
    void trim(std::string& str);
    std::string extractValue(const std::string& json, const std::string& key);
};

#endif  // JSON_PARSER_H_
