/*
  KeyValue.h 
  
*/
#include <Arduino.h>
#ifndef __KEYVALUE_H_
#define __KEYVALUE_H_

class KeyValue {
    public:
      KeyValue(String key, String value);
      String getKey(); // returns key to memory chuck, used for searching through collections of objects
      void setValue(value); // sets the value of the memory chunk
      
    private:

};
#endif