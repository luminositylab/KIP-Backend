#include <StepperController.h>
#include <DevBoardLayout.h>
#include "BusManager.h"
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include <ArduinoSTL.h>
#include <ArduinoUnit.h>
#include "KeyValue.h"

unsigned long lastTime;
unsigned long dt;
test(ok) 
{
  int x=3;
  int y=3;
  assertEqual(x,y);
}

BusManager _busManager = BusManager();
void setup() {
    Serial.begin(9600); // <---- DO NOT RUN SERIAL 
    lastTime = micros();
}

void loop() {
  lastTime = micros();
  _busManager.update(dt);
  dt = micros() - lastTime;
}