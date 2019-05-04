/*
  KeyValue.h 
  
*/
#include <Arduino.h>
#include <ArduinoSTL.h>
#ifndef __KEYVALUE_H_
#define __KEYVALUE_H_

enum Type {
    Number,
    Str
};
class KeyValue {
    public:
      KeyValue(std::string key, std::string value);
      std::string getKey(); // returns key to memory chuck, used for searching through collections of objects
      void setValue(std::string value); // sets the value of the memory chunk 
      int test();
    private:
      Type _inferredType; // keeps track of the inferred data type of the memory
      std::string _key; // stores the key to the memory chunk
      std::string _value; // stores the value of the key in the memory chunk`
      float _fNumber; // if the value is a float, then this will be defined and returned
      int _iNumber; // if the value is an integer, this will be defined and returned

};
#endif