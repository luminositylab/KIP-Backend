//Sensor and Servo Control Program
//Designed for the KIP robot
//(C) 2018 The Luminosity Lab

//This program reads multi-byte serial inputs.
//A 'use packet' bit in the first byte indicates whether to ignore or use the packet.
//A 'function' bit in the first byte indicates which of the following functions will be used:
//functionBit = 0: Report sensor data.
//functionBit = 1: Set servo positions.
//The number of bytes expected per packet can be specified below.
//Sensor data is monitored with a running average of depth specified below.
//Servo positions are specified in degrees from 0 to 180.
//8 digital input pullup pins are available.

//Include necessary libraries:
#include <Servo.h>

//Declaration of program constants:
const int numberOfBytes = 6;                    //Number of bytes to expect per packet
const byte usePacketBit = 0;                    //Specifies which bit in Byte 0 is 'use packet'
const byte functionBit = 1;                     //Specifies which bit in Byte 0 is 'function'
const byte servoByte0 = 1;                      //Specifies byte containing angle for servo0
const byte servoByte1 = 2;                      //Specifies byte containing angle for servo1
const byte servoByte2 = 3;                      //Specifies byte containing angle for servo2
const byte servoByte3 = 4;                      //Specifies byte containing angle for servo3
const bool activeHigh = 1;                      //Set 1 to utilize packet when 'use packet' bit is 1
const unsigned long  serialTimeout = 100000;    //Specifies mid-packet timeout (in us) before byte counter resets to 0.
const long baudRate = 57600;                    //Specifies serial baud rate

//Declaration of timing variables:
unsigned long timeoutElapsed = 0;               //Packet timeout elapsed time variable
unsigned long timeoutLastMicros = 0;            //Packet timeout last microseconds value

//Declaration of Servo objects:
Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;

//Assignment of pin numbers:
const byte servoPin0 = 7;
const byte servoPin1 = 8;
const byte servoPin2 = 9;
const byte servoPin3 = 10;
const byte sesnor0 = 3;
const byte sesnor1 = 4;
const byte sesnor2 = 5;
const byte sesnor3 = 6;
const byte sesnor4 = 11;
const byte sesnor5 = 12;
const byte sesnor6 = 13;
const byte sesnor7 = 14;

//Declaration of program control variables:
byte receivedByte = 0;              //Number of the byte received from the packet
byte dataContents = 0;              //Contents of the most recent serial byte
bool usePacket = 0;                 //'Use Packet' bit
bool function = 0;                  //Function selection choice

void setup() {
  //Activate serial communication:
  Serial.begin(baudRate);

  //Attach servo control objects:
  servo0.attach(servoPin0);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);

  //Set pin modes for ultrasonic sensors:
  pinMode(sesnor0, INPUT_PULLUP);
  pinMode(sesnor1, INPUT_PULLUP);
  pinMode(sesnor2, INPUT_PULLUP);
  pinMode(sesnor3, INPUT_PULLUP);
  pinMode(sesnor4, INPUT_PULLUP);
  pinMode(sesnor5, INPUT_PULLUP);
  pinMode(sesnor6, INPUT_PULLUP);
  pinMode(sesnor7, INPUT_PULLUP);
  
}

void checkPacketTimeout() { //Determine if the mid-packet timeout has been reached:
  //Compute elapsed time:
  timeoutElapsed = micros() - timeoutLastMicros;
  if(timeoutElapsed >= serialTimeout)
  {
    //Reset packet counter:
    receivedByte = 0;
    //Reset timing:
    timeoutLastMicros = micros();
  }
}

void setServoPositions() {  //Set servo positions:
    //Confine angle to 0-180 degrees:
    if(dataContents > 180)
    {
      dataContents = 180;
    }
    if(dataContents < 0)
    {
      dataContents = 0;
    }

    //Determine if the latest byte contains data for servo0:
    if(receivedByte == servoByte0)
    {
      servo0.write(dataContents);
    }

    //Determine if the latest byte contains data for servo1:
    if(receivedByte == servoByte1)
    {
      servo1.write(dataContents);
    }
    
    //Determine if the latest byte contains data for servo2:
    if(receivedByte == servoByte2)
    {
      servo2.write(dataContents);
    }

    //Determine if the latest byte contains data for servo3:
    if(receivedByte == servoByte3)
    {
      servo3.write(dataContents);
    }
}

void reportSensorData() {     //Report sensor data to serial port (if applicable):
  //Write digital inputs from sensor pins to serial:
  Serial.write(digitalRead(sesnor0));
  Serial.write(digitalRead(sesnor1));
  Serial.write(digitalRead(sesnor2));
  Serial.write(digitalRead(sesnor3));
  Serial.write(digitalRead(sesnor4));
  Serial.write(digitalRead(sesnor5));
  Serial.write(digitalRead(sesnor6));
  Serial.write(digitalRead(sesnor7));
}

void checkSerial() {                //Determine if data is in the serial buffer and utilize it:
  while(Serial.available() > 0)
  {
    //Reset mid-packet timeout timer:
    timeoutLastMicros = micros();

    //Load most recent byte into dataContents:
    dataContents = Serial.read();
    
    //Determine if the latest byte is the first byte in the packet:
    if(receivedByte == 0)
    {
      //Determine if packet should be used:
      usePacket = (bitRead(dataContents, usePacketBit) == activeHigh);

      //Determine packet function:
      function = bitRead(dataContents, functionBit);

      //Report sensor data to serial port (if applicable):
      if(usePacket && (!function))
      {
        reportSensorData();
      }
    }

    //Set servo positions (if applicable):
    if(usePacket && function)
    {
      setServoPositions();
    }

    //Increment value of receivedByte:
    receivedByte++;
    
    //Determine if packet is finished:
    if(receivedByte >= numberOfBytes)
    {
      //If packet is finished, reset receivedByte counter:
      receivedByte = 0;
    }
  }
}

void loop() {
  //Reset packet counter if mid-packet timeout is reached
  checkPacketTimeout();
  
  //Determine if data is in the serial buffer and utilize it:
  checkSerial();
}
