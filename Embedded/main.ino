// main.ino
// Main Arduino C code for running controller. 
// Written by G M Rowland // g@gmichaelrowland.com


#include <StepperController.h>
#include <Arduino.h>

unsigned long lastTime;
unsigned long dt;

void setup() {


}

void loop() {
	lastTime = micros();
	// Run code
	dt = micros() - lastTime;
}
