#include "ParseOption.h"

ParseOption::ParseOption() {
}

/**
 * This method checks to see if a key value chuck exists with a matching key.
*/
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


/**
 * This method loops through the storage and checks if any of the key value chunks match the given key.
 * Returns the value if the key is found and if no key matches, it will return an empty string.
*/
std::string ParseOption::get(std::string key) {
    for (KeyValue &kv : _storage) {
        if (kv.getKey() == key) {
            return kv.getValue();
        }
    }
    return std::string();
}


/**
 * This method sets the key to the value in the storage vector.
*/
void ParseOption::set(std::string key, std::string value) {
    _storage.push_back(KeyValue(key, value));
}

/**
 * Adds a char from the i2c bus to the pool of character and parses it for commands. 
 * 
*/
void ParseOption::addCharToPool(char c) {
    _charPool += c; // _charPool is a std::string, not a std::vector
    // std::cout << _charPool << std::endl;
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
    } else if (commandArguments.size() >= 3 && commandArguments.at(0) == "GET") {
        // pass the command's arguemnts to the correct method for return.
        
        return true;
    } else {
        return false;
    }
}

/**
 * Parses the command instruction pool for the SET keyword. Expects the following syntax:
 * SET <key> <value>
 * Requires terminal space in order to excecute.
*/
bool ParseOption::parseSet() {
    std::vector<std::string> commandArguments = getArgs("SET", 3);
    if (commandArguments.empty()) {
        return false;
    } else if (commandArguments.size() == 3 + 1 && commandArguments.at(0) == "SET") {
        set( commandArguments.at(1), commandArguments.at(2) ); // sets the key value in storage.
        return true;
    } else {
        return false;
    }
}


/**
 * This method parses the character pool for the applicable commands. Exits the parse when it finds a command with sufficient structure.
 * 
*/
void ParseOption::parsePool() {
    
}


/**
 * This method is used to parse the character pool string for the given keyword command and number of arguments expected.
 * This makes sure to account for the teminal space as an argument to seperate the command from others.
 * Example:
 * SET foo bar <-- would be evaluated to a argument list with: getArgs("SET", 3) because there are three arguments
 * number of arugments includes the command name.
 * 
*/
std::vector<std::string> ParseOption::getArgs(std::string keyword, unsigned short expectedArgumentSize) {
    std::size_t found = _charPool.find(keyword + " ");
    if (found == std::string::npos) {
        return std::vector<std::string>();
    }
    std::vector<std::string> results = split(_charPool, " "); // tokenize the string by spaces.
    // Adds one to the expectedArgumentSize so account for the space.
    if (results.size() >= expectedArgumentSize + 1 && results.at(0) == keyword) {
        _charPool.clear();
        return results;
    } else {
        return std::vector<std::string>();
    }
}


/**
 * This is a helper method that is used to split up strings into sections by other deliminator strings.
 * Example, split("hello world", " ") yields <"hello","world"> in vector form.
*/
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


/**
 * This method is used to test the functionality of the ParseOption class.
 * 
*/
// void ParseOption::runUnitTests() {
//     std::cout << "[SET TEST]:";
//     _charPool = "SET foo bar ";
//     if (parseGet()) {
//         std::cout << "[FAILED]" << std::endl;
//     } else if (parseSet()) {
//         std::cout << "[PASSED]" << std::endl;
//     } else {
//         std::cout << "[FAILED]" << std::endl;
//     }

//     std::cout << "[POOL EMPTY TEST]:";
//     if (_charPool.empty()) {
//         std::cout << "[PASSED]" << std::endl;
//     } else {
//         std::cout << "[FAILED]" << std::endl;
//     }

//     _charPool = std::string();
//     std::cout << "[EMPTY PARSE TEST]:";
//     if (parseGet()) {
//         std::cout << "[FAILED]" << std::endl;
//     } else if (parseSet()) {
//         std::cout << "[FAILED]" << std::endl;
//     } else {
//         std::cout << "[PASSED]" << std::endl;
//     }

//     _charPool = "GET foo ";
//     std::cout << "[GET TEST]:";
//     if(parseSet()) {
//         std::cout << "[FAILED]" << std::endl;
//     } else if (parseGet()) {
//         std::cout << "[PASSED]" << std::endl;
//     } else {
//         std::cout << "[FAILED]" << std::endl;
//     }
//     _charPool = std::string();
// }