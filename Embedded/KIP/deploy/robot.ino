#include <StepperController.h>
#include <DevBoardLayout.h>
#include "BusManager.h"
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include <ArduinoSTL.h>

unsigned long lastTime;
unsigned long dt;

//BusManager _busManager = BusManager();
ParseOption _parser = ParseOption();
void setup() {
  Serial.begin(9600);
  std::cout << "Feed me an integers." << std::endl;
  _parser.runUnitTests();
    // Wire.begin(0x8);
    // Wire.onReceive(receiveEvent);
    // lastTime = micros();
}

void loop() {
  lastTime = micros();
  // _busManager.update(dt);
  // Serial.println("hello");
  
  dt = micros() - lastTime;
  delay(200);
}


// void receiveEvent(int howMany) {
//   while (Wire.available()) { // loop through all but the last
//     char c = Wire.read(); // receive byte as a character
//     _busManager.feedData(c);
//   }
// }