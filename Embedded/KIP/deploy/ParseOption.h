/*
  ParseOption.h 
  ParseOption is a key-value database within the Arduino memory that parses the I2C bus for database commands
  and stores the data in allocated non-persistent memory. Database commands are case sensitive. (GET != get)
  
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
#include <sstream>

#ifndef __PARSEOPTION_H_
#define __PARSEOPTION_H_

class ParseOption {
    public:
      ParseOption();
      void testUpdate(); // calls some private methods for testing purposes
      bool keyExists(std::string key); // checks if the KeyValue exists based on key token
      std::string get(std::string key); // returns the value of a specific key. if the key doesn't exist, returns an unstable KeyValue
      void set(std::string key, std::string value); // creates a new key value chunk if one doesn't already exist. returns true if memory allocated.
      void addCharToPool(char c); // adds a char to the pool of chars that gets parsed for database commands.
    private:
      std::string _charPool; // this is the pool of characters that gets parsed for commands.
      std::vector<KeyValue> _storage; // this acts as the persistent memory of the robot.
      void parsePool(); // parses the pool of characters for database commands.
      std::vector<std::string> split(const std::string &s, char delim); // returns vector of strings split by deliminator
      // ---- subparse operations ---
      bool parseGet(); // parses pool for get command, returns true if the command finds a get command
      bool parseSet(); // parses pool for set command, returns true if the command finds a set command
      
};
#endif