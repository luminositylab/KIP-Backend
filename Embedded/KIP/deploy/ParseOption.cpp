#include "ParseOption.h"

ParseOption::ParseOption() {
    _charPool = "GET TEST";
}

bool ParseOption::keyExists(std::string key) {
    if (_storage.size() == 0) {
        return false;
    }
    for (KeyValue &kv : _storage) {
        if (kv.getKey() == key) {
            return true;
        }
    }
    return false;
}

std::string ParseOption::get(std::string key) {
    for (KeyValue &kv : _storage) {
        if (kv.getKey() == key) {
            return kv.getValue();
        }
    }

    if (!keyExists(key)) {
        return std::string();
    }
}

void ParseOption::set(std::string key, std::string value) {
    _storage.push_back(KeyValue(key, value));
}

void ParseOption::addCharToPool(char c) {
    _charPool.append(c);
    parsePool();
}

bool ParseOption::parseGet() {
    Serial.println("works2");

    std::size_t found = _charPool.find("GET ");
    if (found == std::string::npos) {
        return false;
    }
    std::vector<std::string> results = split(_charPool, " ");
    for (auto s : results) {
        // std::cout << s << std::endl;
    }
    Serial.println("works3");

    return true;
}
bool ParseOption::parseSet() {

}
void ParseOption::parsePool() {
    if (parseGet()) {
        
    }
}

std::vector<std::string> ParseOption::split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;
    while (std::getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

void ParseOption::testUpdate() {
    Serial.println("works");
    parseGet();
}