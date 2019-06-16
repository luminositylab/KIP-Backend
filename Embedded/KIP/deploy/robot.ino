#include <StepperController.h>
#include <DevBoardLayout.h>
#include "BusManager.h"
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include <ArduinoSTL.h>

unsigned long lastTime;
unsigned long dt;

BusManager _busManager = BusManager(); 
// ParseOption _parser;
void setup() {
  _busManager._robotState.begin();
  // _parser = ParseOption();
  Serial.begin(250000);
  Wire.setClock(400000);
  std::cout << "[BOOTED]" << std::endl;
  // _parser.runUnitTests();
    Wire.begin(0x8);
    Wire.onReceive(receiveEvent);
    lastTime = micros();
}

void loop() {
  lastTime = micros();
  _busManager.update(dt);
  // Serial.println("hello");
  
  dt = micros() - lastTime;
}


void receiveEvent(int howMany) {
  // std::cout << "got stuff!" << std::endl;
  int i = 0;
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    std::cout << (int)c  << std::endl;

    _busManager.feedData(c);
    // std::cout << "[yee]" << std::endl;

  }
}