// main.ino
// Main Arduino C code for running controller. 
// Written by G M Rowland // g@gmichaelrowland.com


#include <StepperController.h>
#include <DevBoardLayout.h>
#include <Arduino.h>
unsigned long lastTime;
unsigned long dt;
DevBoardLayout _dev;
void setup() {
	_dev = 

}

void loop() {
	lastTime = micros();
	// Run code
	dt = micros() - lastTime;
}
