#include <StepperController.h>
#include <DevBoardLayout.h>
#include "BusManager.h"
#include <Arduino.h>
#include <Wire.h>

unsigned long lastTime;
unsigned long dt;
BusManager _busManager = BusManager();

void setup() {
    Wire.begin(0x8);
    Wire.onReceive(receiveEvent);
    lastTime = micros();
}

void loop() {
  lastTime = micros();
  _busManager.update(dt);
  dt = micros() - lastTime;
}

void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    _busManager.feedData(c);
  }
}