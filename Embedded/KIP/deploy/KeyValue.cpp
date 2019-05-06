#include "KeyValue.h"
#include <ArduinoSTL.h>

KeyValue::KeyValue(std::string key, std::string value) {
    _key = key;
    _value = value;
    _inferredType = inferType(value);
}


Type KeyValue::inferType(std::string val) {
    return Str; // by default, returns string. Change in future.
}

std::string KeyValue::getKey() {
    return _key;
}


void KeyValue::setValue(std::string value) {
    _value = value;
    _inferredType = inferType(value);
}

std::string KeyValue::getValue() {
    return _value;
}