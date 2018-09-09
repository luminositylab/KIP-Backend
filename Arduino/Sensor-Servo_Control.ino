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
//4 sensors are specified and their distance averages are reported as double-floats.

//Include necessary libraries:
#include <Servo.h>

//Declaration of program constants:
const int numberOfBytes = 6;        //Number of bytes to expect per packet
const byte usePacketBit = 0;        //Specifies which bit in Byte 0 is 'use packet'
const byte functionBit = 1;         //Specifies which bit in Byte 0 is 'function'
const byte servoByte0 = 1;          //Specifies byte containing angle for servo0
const byte servoByte1 = 2;          //Specifies byte containing angle for servo1
const byte servoByte2 = 3;          //Specifies byte containing angle for servo2
const byte servoByte3 = 4;          //Specifies byte containing angle for servo3
const bool activeHigh = 1;          //Set 1 to utilize packet when 'use packet' bit is 1
const long baudRate = 250000;       //Specifies serial baud rate

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
const byte resetPacketPin = 18;
const byte trigPin0 = 3;
const byte echoPin0 = 4;
const byte trigPin1 = 5;
const byte echoPin1 = 6;
const byte trigPin2 = 11;
const byte echoPin2 = 12;
const byte trigPin3 = 13;
const byte echoPin3 = 14;
const byte trigPin4 = 15;
const byte echoPin4 = 16;
const byte trigPin5 = 17;
const byte echoPin5 = 2;

//Declaration of program control variables:
byte receivedByte = 0;              //Number of the byte received from the packet
byte dataContents = 0;              //Contents of the most recent serial byte
bool usePacket = 0;                 //'Use Packet' bit
bool function = 0;                  //Function selection choice

//Declaration of sensor data acquisition constants and variables:
const unsigned long  sensorPulseDelay = 10;   //Pulse delay for ultrasonic sensor (us)
const unsigned long sensorTimeout = 5000;     //Sensor timeout (us)
const int runningArraySize = 10;              //Number of most recent data samples to use in the running average
const double soundConstant = 0.0343;          //Speed of sound constant (cm/us)

double latestDistance = 0;         //Variable for latest detected distance

//Running average arrays:
double runningAverageArray0[runningArraySize] = {0};
double runningAverageArray1[runningArraySize] = {0};
double runningAverageArray2[runningArraySize] = {0};
double runningAverageArray3[runningArraySize] = {0};
double runningAverageArray4[runningArraySize] = {0};
double runningAverageArray5[runningArraySize] = {0};

int runningAverageCounter0 = 0;     //Counter for running average 0
int runningAverageCounter1 = 0;     //Counter for running average 1
int runningAverageCounter2 = 0;     //Counter for running average 2
int runningAverageCounter3 = 0;     //Counter for running average 3
int runningAverageCounter4 = 0;     //Counter for running average 4
int runningAverageCounter5 = 0;     //Counter for running average 5

void setup() {
  //Activate serial communication:
  Serial.begin(baudRate);

  //Attach servo control objects:
  servo0.attach(servoPin0);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);

  //Set pin modes for ultrasonic sensors:
  pinMode(trigPin0, OUTPUT);
  pinMode(echoPin0, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);
  pinMode(trigPin5, OUTPUT);
  pinMode(echoPin5, INPUT);

  //Set all ultrasonic sensor outputs:
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  digitalWrite(trigPin3, LOW);
  digitalWrite(trigPin4, LOW);
  digitalWrite(trigPin5, LOW);
  
}

void setServoPositions() {            //Set servo positions:
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

double computeRunningAverage(int arrayNumber) {    //Compute the running average
  //Declare sum variable:
  double averageSum = 0;

  //Determine the sensor number to use for the calculation:
  if(arrayNumber == 0)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray0[index];
    }
  }
  if(arrayNumber == 1)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray1[index];
    }
  }
  if(arrayNumber == 2)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray2[index];
    }
  }
  if(arrayNumber == 3)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray3[index];
    }
  }
  if(arrayNumber == 4)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray4[index];
    }
  }
  if(arrayNumber == 5)
  {
    //Iterate over all elements in the array:
    for(int index = 0; index < runningArraySize; index++)
    {
      //Add array content to sum:
      averageSum += runningAverageArray5[index];
    }
  }

  //Return the average:
  return averageSum/runningArraySize;
}

void readSensors() {          //Perform sensor data acquisition:
  //Read input from sensor 0:
  digitalWrite(trigPin0, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin0, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin0, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray0[runningAverageCounter0] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter0++;
  if(runningAverageCounter0 > runningArraySize)
  {
    runningAverageCounter0 = 0;
  }

  //Read input from sensor 1:
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin1, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin1, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray1[runningAverageCounter1] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter1++;
  if(runningAverageCounter1 > runningArraySize)
  {
    runningAverageCounter1 = 0;
  }

  //Read input from sensor 2:
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin2, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin2, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray2[runningAverageCounter2] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter2++;
  if(runningAverageCounter2 > runningArraySize)
  {
    runningAverageCounter2 = 0;
  }

  //Read input from sensor 3:
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin3, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin3, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray3[runningAverageCounter3] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter3++;
  if(runningAverageCounter3 > runningArraySize)
  {
    runningAverageCounter3 = 0;
  }

  //Read input from sensor 4:
  digitalWrite(trigPin4, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin4, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin4, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray4[runningAverageCounter4] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter4++;
  if(runningAverageCounter4 > runningArraySize)
  {
    runningAverageCounter4 = 0;
  }

  //Read input from sensor 5:
  digitalWrite(trigPin5, HIGH);
  delayMicroseconds(sensorPulseDelay);
  digitalWrite(trigPin5, LOW);
  //Wait for latest distance:
  latestDistance = pulseIn(echoPin5, HIGH, sensorTimeout)*soundConstant/2;
  //Report distance into running average:
  runningAverageArray5[runningAverageCounter5] = latestDistance;
  //Increment array counter and determine if it should be reset.
  runningAverageCounter5++;
  if(runningAverageCounter5 > runningArraySize)
  {
    runningAverageCounter5 = 0;
  }

}

void reportSensorData() {     //Report sensor data to serial port (if applicable):
  
  //Write sensor data to serial:
  Serial.println(String(computeRunningAverage(0),8));
  Serial.println(String(computeRunningAverage(1),8));
  Serial.println(String(computeRunningAverage(2),8));
  Serial.println(String(computeRunningAverage(3),8));
  Serial.println(String(computeRunningAverage(4),8));
  Serial.println(String(computeRunningAverage(5),8));
}

void checkPacketReset() {           //Reset packet counter if resetPacketPin has been pulled high
  if(digitalRead(resetPacketPin))
  {
    receivedByte = 0;
  }
}

void checkSerial() {                //Determine if data is in the serial buffer and utilize it:
  if(Serial.available() > 0)
  {
    dataContents = Serial.read();   //Load most recent byte into dataContents
    
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
  //Reset packet counter if resetPacketPin has been pulled high:
  checkPacketReset();
  
  //Determine if data is in the serial buffer and utilize it:
  checkSerial();
  
  //Perform sensor data acquisition:
  readSensors();
}
