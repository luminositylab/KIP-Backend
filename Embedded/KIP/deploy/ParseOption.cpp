#include "ParseOption.h"

ParseOption::ParseOption() {
    _charPool = "SET foo bar ";
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

/**
 * This method parses the pool of characters that are collected form the i2c bus and tokenizes the pool
 * by spaces and makes sure the the GET_ <-- with space keyword exists and that the last character in the string
 * is a space to indicate the end of a command process.
 * 
*/
bool ParseOption::parseGet() {
    std::vector<std::string> commandArguments = getArgs("GET", 2); // returns the command arguments if there are any, if not it will return and empty vector
    if (commandArguments.empty()) {
        return false;
    } else if (commandArguments.size() == 2 && commandArguments.at(0) == "GET") {
        // pass the command's arguemnts to the correct method for return.
        
        return true;
    } else {
        return false;
    }
}
bool ParseOption::parseSet() {
    // std::cout << "set begin" << std::endl;
    std::vector<std::string> commandArguments = getArgs("SET", 3);
    std::cout << "These are the commands:" << std::endl;
    for (std::string s : commandArguments) {
        std::cout << s << std::endl;
    }
    if (commandArguments.empty()) {
        return false;
    } else if (commandArguments.size() >= 3 && commandArguments.at(0) == "SET") {
        // store the set in key value database
        std::cout << "found command" << std::endl;
        _storage.push_back( KeyValue(commandArguments.at(1), commandArguments.at(2)) );
        return true;
    } else {
        std::cout << "couldn't find anything in set" << std::endl;
        return false;
    }
}
void ParseOption::parsePool() {
    if (parseGet()) {
        return;
    }
    if (parseSet()) {
        return;
    }
}



// this method will clear the parsing pool if the conditions for a successful command parse are met. 
std::vector<std::string> ParseOption::getArgs(std::string keyword, unsigned short expectedArgumentSize) {
    std::size_t found = _charPool.find(keyword + " ");
    if (found == std::string::npos /*|| _charPool.substr(_charPool.size()-2, 1) != " "*/) { // if the position wasn't found, return false to the parse op
    // also returns false when the last character in the string is a space. this indicates that the command is not complete/ready for processing
        return std::vector<std::string>();
    }
    std::vector<std::string> results = split(_charPool, " "); // tokenize the string by spaces.
    if (results.size() >= expectedArgumentSize && results.at(0) == keyword) {

        _charPool.clear();
        return results;
    } else {
        return std::vector<std::string>();
    }
}

std::vector<std::string> ParseOption::split (std::string &s, std::string delimiter) {
    std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;
    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
void ParseOption::testUpdate() {
    std::cout << "testing update" << std::endl;
    // if (parseGet()) {
    //     std::cout << "get parsed" << std::endl;
    // }
    if (parseSet()) {
        std::cout << "set parsed" << std:: endl;
    }
}