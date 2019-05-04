/*
  ParseOption.h 
  ParseOption is a key-value database within the Arduino memory that parses the I2C bus for database commands
  and stores the data in allocated non-persistent memory. Database commands are not case sensitive. (GET == get)
  
  I2C bus command structure:
  --- simple key value ---
  SET key value
  GET key
  --- set manipulation ---
  SADD key value
  SREMOVE key
  SPOP key
  SMEMBERS key
  --- database managment ---
  MEM - returns memory usage of database
  

  --- persistent memory managment ---
  --- uses EEPROM in ATMEGA to make some values persist to reboot
  PSET key value
  PGET key

  Values are not type safe as to safe bus bandwidth, the parses infers type.
  Inferred types are: String, Number
*/
#include <Arduino.h>
#include "KeyValue.h"
#include <ArduinoSTL.h>
#ifndef __PARSEOPTION_H_
#define __PARSEOPTION_H_

class ParseOption {
    public:
      String parseBlock;
      bool keyExists(String key);
      KeyValue get(String key);
      bool set(String key, String value); // creates a new key value chunk if one doesn't already exist. returns true if memory allocated.
      bool addCharToPool(char c); // adds a char to the pool of chars that gets parsed for database commands.

    private:
      std::vector<KeyValue> _storage;
      void parsePool(); // parses the pool of characters for database commands.

};
#endif