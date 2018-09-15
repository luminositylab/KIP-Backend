#include <Servo.h>
#define numServos 4
#define numlimits 4

int servoPins[numServos] = {3,5,6,9};
int limitPins[numlimits] = {};

Servo servos[numServos];

int baudRate = 9600;

int msUsDelay = 9000;//compensated delay for 100Hz

void setup() {
  Serial.begin(baudRate);
  for (int s = 0; s < numServos; s++)
    servos[s].attach(servoPins[s])

  for (int l = 0; l < numlimits; l++)
    pinMode(limitPins[l], INPUT);
}


void loop() {
  if (Serial.available() > 0)// take in commands
    while (Serial.available() > 0)
      if (Serial.read() == 0)
        for (int s = 0; s < numServos; s++)
          servos[s].write(Serial.read());

    for (int l = 0; l < numlimits; l++)
      Serial.println("%d:%d" l, digitalRead(limitPins[l]))

  delayMicroseconds(msUsDelay);
}
