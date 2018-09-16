//Timer-based stepper motor driver for MYRA/KIP robot
//Buffered version
//(C) 2018 The Luminosity Lab

//This program controls the H-bridges for 4 stepper motors.
//It utilizes elapsed timers to control operation of the motors, with 100-ms "bursts" of runtime
//Packets sent to the microcontroller contain sequences of step counts.

//The serial UART input also features an identification byte (Byte 0 in the packet).
//One bit in this byte determines if this microcontroller should utilize the packet (for parallel-Rx systems).
//One bit in this byte determines whether pulse-driving should be used at lower speeds (to save power).
//One bit in this byte determines in which direction the motor to be controlled should rotate.
//Two bits in this byte determine the number of the motor to be controlled.
//One bit in this byte determines if the packet data should be force-used and the buffer to be cleared.

//Pulse-driving is implemented for speeds below a minimum value.
//Three different pulse widths are supported for different speeds in order to maximize torque at all levels.

//Declaration of program constants:
const int numberOfBytes = 6;                  //Number of bytes to expect per packet
const byte usePacketBit = 0;                  //Specifies which bit in Byte 0 is 'use packet'
const byte usePulseBit = 1;                   //Specifies which bit in Byte 0 is 'use pulse-driving'
const byte directionBit = 2;                  //Specifies which bit in Byte 0 is 'direction'
const byte motorNumber0Bit = 3;               //Specifies which bit in Byte 0 is first to indicate which motor is being controlled
const byte motorNumber1Bit = 4;               //Specifies which bit in Byte 0 is second to indicate which motor is being controlled
const byte forcePacketBit = 5;                //Specifies which bit in Byte 0 forces application of packet contents
const bool activeHigh = 0;                    //Set 0 to utilize packet when 'use packet' bit is 0
const long baudRate = 57600;                  //Specifies serial baud rate
const int pulseCriteria = 20;                 //Specifies minimum steps value for which to enable pulse-driving
const unsigned long pulseWidthLow = 10000;    //Specifies the width of pulse-driving pulses for low speeds (in us)
const unsigned long pulseWidthMed = 5000;     //Specifies the width of pulse-driving pulses for medium speeds (in us)
const unsigned long pulseWidthHigh = 2500;    //Specifies the width of pulse-driving pulses for high speeds (in us)
const int lowCriteria = 3;                    //Specifies steps value for low speed
const int medCriteria = 7;                    //Specifies steps value for medium speed
const int highCriteria = 20;                  //Specifies steps value for high speed
const int bufferSize = 250;                   //Size of data buffer
const unsigned long timeForSteps = 100000;    //Time (in us) for each buffer read cycle cycle
const unsigned long  serialTimeout = 100000;  //Specifies mid-packet timeout (in us) before byte counter resets to 0.

//Elapsed timer variables:
unsigned long timeoutElapsed = 0;             //Packet timeout elapsed time variable
unsigned long timeoutLastMicros = 0;          //Packet timeout last microseconds value
unsigned long cycleTimerElapsed = 0;          //Motor update cycle elapsed time variable
unsigned long cycleTimerLastMicros = 0;       //Motor update cycle last microseconds value
unsigned long motor0_elapsed = 0;
unsigned long motor0_lastMicros = 0;
unsigned long motor1_elapsed = 0;
unsigned long motor1_lastMicros = 0;
unsigned long motor2_elapsed = 0;
unsigned long motor2_lastMicros = 0;
unsigned long motor3_elapsed = 0;
unsigned long motor3_lastMicros = 0;

//Assignment of pin numbers:
const byte pin0 = 2;
const byte pin1 = 3;
const byte pin2 = 4;
const byte pin3 = 5;
const byte pin4 = 6;
const byte pin5 = 7;
const byte pin6 = 8;
const byte pin7 = 9;
const byte pin8 = 10;
const byte pin9 = 11;
const byte pin10 = 12;
const byte pin11 = 13;
const byte pin12 = 14;
const byte pin13 = 15;
const byte pin14 = 16;
const byte pin15 = 17;

//Declaration of program variables:
byte receivedByte = 0;              //Number of the byte received from the packet
byte dataContents = 0;              //Contents of the most recent serial byte
bool usePacket = 0;                 //'Use Packet' bit
bool usePulseDrive = 0;             //'Use Pulse-Driving' bit
bool directionData = 0;             //'Direction' bit
bool forcePacket = 0;               //'Force packet application' bit
byte motor0_stepsPerCycle = 0;      //Number of steps per buffer read cycle (motor 0) 
byte motor1_stepsPerCycle = 0;      //Number of steps per buffer read cycle (motor 1)
byte motor2_stepsPerCycle = 0;      //Number of steps per buffer read cycle (motor 2)
byte motor3_stepsPerCycle = 0;      //Number of steps per buffer read cycle (motor 3)
bool motor0_Direction = 0;          //Direction for motor0
bool motor1_Direction = 0;          //Direction for motor1
bool motor2_Direction = 0;          //Direction for motor2
bool motor3_Direction = 0;          //Direction for motor3
bool motor0_UsePulse_Data = 0;      //Packet data to use pulse-driving for motor0
bool motor1_UsePulse_Data = 0;      //Packet data to use pulse-driving for motor1
bool motor2_UsePulse_Data = 0;      //Packet data to use pulse-driving for motor2
bool motor3_UsePulse_Data = 0;      //Packet data to use pulse-driving for motor3
bool motor0_UsePulse = 0;           //Use pulse-driving for motor0
bool motor1_UsePulse = 0;           //Use pulse-driving for motor1
bool motor2_UsePulse = 0;           //Use pulse-driving for motor2
bool motor3_UsePulse = 0;           //Use pulse-driving for motor3
byte motor0_CurrentStep = 0;        //Current step for motor0
byte motor1_CurrentStep = 0;        //Current step for motor1
byte motor2_CurrentStep = 0;        //Current step for motor2
byte motor3_CurrentStep = 0;        //Current step for motor3
byte motor0_CurrentPulseStep = 0;   //Current pulse-driving step for motor0
byte motor1_CurrentPulseStep = 0;   //Current pulse-driving step for motor1
byte motor2_CurrentPulseStep = 0;   //Current pulse-driving step for motor2
byte motor3_CurrentPulseStep = 0;   //Current pulse-driving step for motor3
unsigned long motor0_PulseWidth = 0;//Specifies the width of pulse-driving pulses for motor0
unsigned long motor1_PulseWidth = 0;//Specifies the width of pulse-driving pulses for motor1
unsigned long motor2_PulseWidth = 0;//Specifies the width of pulse-driving pulses for motor2
unsigned long motor3_PulseWidth = 0;//Specifies the width of pulse-driving pulses for motor3
byte packetMotorNumber = 0;         //Specifies the motor number to which the packet applies.
int buffer0WriteIndex = 0;          //Write index for motor 0 buffers
int buffer1WriteIndex = 0;          //Write index for motor 1 buffers
int buffer2WriteIndex = 0;          //Write index for motor 2 buffers
int buffer3WriteIndex = 0;          //Write index for motor 3 buffers
int buffer0ReadIndex = 0;           //Read index for motor 0 buffers
int buffer1ReadIndex = 0;           //Read index for motor 1 buffers
int buffer2ReadIndex = 0;           //Read index for motor 2 buffers
int buffer3ReadIndex = 0;           //Read index for motor 3 buffers

//Buffer arrays:
byte stepBuffer0[bufferSize] = {0};
byte stepBuffer1[bufferSize] = {0};
byte stepBuffer2[bufferSize] = {0};
byte stepBuffer3[bufferSize] = {0};
byte directionBuffer0[bufferSize/8+1] = {0};  //Organized into bytes because each [bool] takes 1 byte normally.
byte directionBuffer1[bufferSize/8+1] = {0};
byte directionBuffer2[bufferSize/8+1] = {0};
byte directionBuffer3[bufferSize/8+1] = {0};
byte pulseBuffer0[bufferSize/8+1] = {0};
byte pulseBuffer1[bufferSize/8+1] = {0};
byte pulseBuffer2[bufferSize/8+1] = {0};
byte pulseBuffer3[bufferSize/8+1] = {0};

//Variables used for development (may be removed later):
bool dummyBool = 0;

void setup() {
  //Activation of serial communication:
  Serial.begin(baudRate);
  
  //Specification of pin modes:
  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  pinMode(pin8, OUTPUT);
  pinMode(pin9, OUTPUT);
  pinMode(pin10, OUTPUT);
  pinMode(pin11, OUTPUT);
  pinMode(pin12, OUTPUT);
  pinMode(pin13, OUTPUT);
  pinMode(pin14, OUTPUT);
  pinMode(pin15, OUTPUT);

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

bool cycleTimer() {        //Determine if buffer reading cycle is over
  //Determine if timeForSteps has passed
  cycleTimerElapsed = micros() - cycleTimerLastMicros;
  if(cycleTimerElapsed >= timeForSteps)
  {
    //Reset timing:
    cycleTimerLastMicros = micros();
    return(1);
  }
  else
  {
    return(0);
  }
}

void writeBuffer0(byte latestStep, bool latestDirection, bool latestPulse) {
  //If forcePacket is TRUE, reset buffer write and read counters to bring packet data to the front of the line:
  if(forcePacket)
  {
    buffer0WriteIndex = 0;
    buffer0ReadIndex = 0;
    //Toggle forcePacket LOW now that the buffer has been re-aligned to zero:
    forcePacket = 0;
  }
  
  //Write the contents of variables into the buffers at the appropriate index:
  stepBuffer0[buffer0WriteIndex] = latestStep;
  bitWrite(directionBuffer0[buffer0WriteIndex/8], buffer0WriteIndex%8, latestDirection);
  bitWrite(pulseBuffer0[buffer0WriteIndex/8], buffer0WriteIndex%8, latestPulse);

  //Increment write index:
  buffer0WriteIndex++;

  //Determine if write index has reached end of buffer:
  if(buffer0WriteIndex >= bufferSize)
  {
    buffer0WriteIndex = 0;
  }
}

void writeBuffer1(byte latestStep, bool latestDirection, bool latestPulse) {
  //If forcePacket is TRUE, reset buffer write and read counters to bring packet data to the front of the line:
  if(forcePacket)
  {
    buffer1WriteIndex = 0;
    buffer1ReadIndex = 0;
    //Toggle forcePacket LOW now that the buffer has been re-aligned to zero:
    forcePacket = 0;
  }
  
  //Write the contents of variables into the buffers at the appropriate index:
  stepBuffer1[buffer1WriteIndex] = latestStep;
  bitWrite(directionBuffer1[buffer1WriteIndex/8], buffer1WriteIndex%8, latestDirection);
  bitWrite(pulseBuffer1[buffer1WriteIndex/8], buffer1WriteIndex%8, latestPulse);

  //Increment write index:
  buffer1WriteIndex++;

  //Determine if write index has reached end of buffer:
  if(buffer1WriteIndex >= bufferSize)
  {
    buffer1WriteIndex = 0;
  }
}

void writeBuffer2(byte latestStep, bool latestDirection, bool latestPulse) {
  //If forcePacket is TRUE, reset buffer write and read counters to bring packet data to the front of the line:
  if(forcePacket)
  {
    buffer2WriteIndex = 0;
    buffer2ReadIndex = 0;
    //Toggle forcePacket LOW now that the buffer has been re-aligned to zero:
    forcePacket = 0;
  }
  
  //Write the contents of variables into the buffers at the appropriate index:
  stepBuffer2[buffer2WriteIndex] = latestStep;
  bitWrite(directionBuffer2[buffer2WriteIndex/8], buffer2WriteIndex%8, latestDirection);
  bitWrite(pulseBuffer2[buffer2WriteIndex/8], buffer2WriteIndex%8, latestPulse);

  //Increment write index:
  buffer2WriteIndex++;

  //Determine if write index has reached end of buffer:
  if(buffer2WriteIndex >= bufferSize)
  {
    buffer2WriteIndex = 0;
  }
}

void writeBuffer3(byte latestStep, bool latestDirection, bool latestPulse) {
  //If forcePacket is TRUE, reset buffer write and read counters to bring packet data to the front of the line:
  if(forcePacket)
  {
    buffer3WriteIndex = 0;
    buffer3ReadIndex = 0;
    //Toggle forcePacket LOW now that the buffer has been re-aligned to zero:
    forcePacket = 0;
  }
  
  //Write the contents of variables into the buffers at the appropriate index:
  stepBuffer3[buffer3WriteIndex] = latestStep;
  bitWrite(directionBuffer3[buffer3WriteIndex/8], buffer3WriteIndex%8, latestDirection);
  bitWrite(pulseBuffer3[buffer3WriteIndex/8], buffer3WriteIndex%8, latestPulse);

  //Increment write index:
  buffer3WriteIndex++;

  //Determine if write index has reached end of buffer:
  if(buffer3WriteIndex >= bufferSize)
  {
    buffer3WriteIndex = 0;
  }
}

void readBuffer0() {   //Read and apply the data contained in buffer 0:  
  //Set the latest steps-per-100ms value:
  motor0_stepsPerCycle = stepBuffer0[buffer0ReadIndex];
  
  //Set direction:
  motor0_Direction = bitRead(directionBuffer0[buffer0ReadIndex/8], buffer0ReadIndex%8);
  
  //Set pulse-driving option:
  motor0_UsePulse_Data = bitRead(pulseBuffer0[buffer0ReadIndex/8], buffer0ReadIndex%8);

  //Increment read index:
  buffer0ReadIndex++;

  //Determine if read index has reached end of buffer:
  if(buffer0ReadIndex >= bufferSize)
  {
    buffer0ReadIndex = 0;
  }
}

void readBuffer1() {   //Read and apply the data contained in buffer 1:
  //Set the latest steps-per-100ms value:
  motor1_stepsPerCycle = stepBuffer1[buffer1ReadIndex];
  
  //Set direction:
  motor1_Direction = bitRead(directionBuffer1[buffer1ReadIndex/8], buffer1ReadIndex%8);
  
  //Set pulse-driving option:
  motor1_UsePulse_Data = bitRead(pulseBuffer1[buffer1ReadIndex/8], buffer1ReadIndex%8);

  //Increment read index:
  buffer1ReadIndex++;

  //Determine if read index has reached end of buffer:
  if(buffer1ReadIndex >= bufferSize)
  {
    buffer1ReadIndex = 0;
  }
}

void readBuffer2() {   //Read and apply the data contained in buffer 2:
  //Set the latest steps-per-100ms value:
  motor2_stepsPerCycle = stepBuffer2[buffer2ReadIndex];
  
  //Set direction:
  motor2_Direction = bitRead(directionBuffer2[buffer2ReadIndex/8], buffer2ReadIndex%8);
  
  //Set pulse-driving option:
  motor2_UsePulse_Data = bitRead(pulseBuffer2[buffer2ReadIndex/8], buffer2ReadIndex%8);
  
  //Increment read index:
  buffer2ReadIndex++;

  //Determine if read index has reached end of buffer:
  if(buffer2ReadIndex >= bufferSize)
  {
    buffer2ReadIndex = 0;
  }
}

void readBuffer3() {   //Read and apply the data contained in buffer 3:
  //Set the latest steps-per-100ms value:
  motor3_stepsPerCycle = stepBuffer3[buffer3ReadIndex];
  
  //Set direction:
  motor3_Direction = bitRead(directionBuffer3[buffer3ReadIndex/8], buffer3ReadIndex%8);
  
  //Set pulse-driving option:
  motor3_UsePulse_Data = bitRead(pulseBuffer3[buffer3ReadIndex/8], buffer3ReadIndex%8);

  //Increment read index:
  buffer3ReadIndex++;

  //Determine if read index has reached end of buffer:
  if(buffer3ReadIndex >= bufferSize)
  {
    buffer3ReadIndex = 0;
  }
}

void checkSerial() {  //Determine if data is in the serial buffer and store it in the data buffer:
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
      if(bitRead(dataContents, usePacketBit) == activeHigh)
      {
        usePacket = 1;
        //Load pulse-drive data from first packet:
        usePulseDrive = bitRead(dataContents, usePulseBit);
        //Load direction data:
        directionData = bitRead(dataContents, directionBit);
        //Load 'force packet' data:
        forcePacket = bitRead(dataContents, forcePacketBit);
        //Load motor number data from first packet:
        if(!bitRead(dataContents, motorNumber1Bit) && !bitRead(dataContents, motorNumber0Bit))
        {
          packetMotorNumber = 0;
        }
        if(!bitRead(dataContents, motorNumber1Bit) && bitRead(dataContents, motorNumber0Bit))
        {
          packetMotorNumber = 1;
        }
        if(bitRead(dataContents, motorNumber1Bit) && !bitRead(dataContents, motorNumber0Bit))
        {
          packetMotorNumber = 2;
        }
        if(bitRead(dataContents, motorNumber1Bit) && bitRead(dataContents, motorNumber0Bit))
        {
          packetMotorNumber = 3;
        }
      }
      else
      {
        usePacket = 0;
      }
    }
  
    //Determine if the latest byte contains data for motor0:
    if(usePacket && (receivedByte != 0) && (packetMotorNumber == 0))
    {
      //Write latest data to the data buffer:
      writeBuffer0(dataContents, directionData, usePulseDrive);
    }
  
    //Determine if the latest byte contains data for motor1:
    if(usePacket && (receivedByte != 0) && (packetMotorNumber == 1))
    {
      //Write latest data to the data buffer:
      writeBuffer1(dataContents, directionData, usePulseDrive);
    }
      
    //Determine if the latest byte contains data for motor2:
    if(usePacket && (receivedByte != 0) && (packetMotorNumber == 2))
    {
      //Write latest data to the data buffer:
      writeBuffer2(dataContents, directionData, usePulseDrive);
    }
  
    //Determine if the latest byte contains data for motor3:
    if(usePacket && (receivedByte != 0) && (packetMotorNumber == 3))
    {
      //Write latest data to the data buffer:
      writeBuffer3(dataContents, directionData, usePulseDrive);
    }
  
    receivedByte++;               //Increment value of receivedByte
      
    //Determine if packet is finished:
    if(receivedByte >= numberOfBytes)
    {
      receivedByte = 0;           //If packet is finished, reset receivedByte counter
    }
  }
}

void checkPulseDrives() {  //Determine if pulse-driving should be used for each motor:
    if(motor0_UsePulse_Data && (motor0_stepsPerCycle <= pulseCriteria))
    {
      motor0_UsePulse = 1;
    }
    else
    {
      motor0_UsePulse = 0;
    }
    if(motor1_UsePulse_Data && (motor1_stepsPerCycle <= pulseCriteria))
    {
      motor1_UsePulse = 1;
    }
    else
    {
      motor1_UsePulse = 0;
    }
    if(motor2_UsePulse_Data && (motor2_stepsPerCycle <= pulseCriteria))
    {
      motor2_UsePulse = 1;
    }
    else
    {
      motor2_UsePulse = 0;
    }
    if(motor3_UsePulse_Data && (motor3_stepsPerCycle <= pulseCriteria))
    {
      motor3_UsePulse = 1;
    }
    else
    {
      motor3_UsePulse = 0;
    }
}

void setPulseWidths() {  //Determine pulse widths for pulse-drive mode:
  //Motor 0:
  //Low criteria:
  if(motor0_stepsPerCycle <= lowCriteria)
  {
    motor0_PulseWidth = pulseWidthLow;
  }
  //Medium criteria:
  else if(motor0_stepsPerCycle <= medCriteria)
  {
    motor0_PulseWidth = pulseWidthMed;
  }
  //High criteria:
  else
  {
    motor0_PulseWidth = pulseWidthHigh;
  }

  //Motor 1:
  //Low criteria:
  if(motor1_stepsPerCycle <= lowCriteria)
  {
    motor1_PulseWidth = pulseWidthLow;
  }
  //Medium criteria:
  else if(motor1_stepsPerCycle <= medCriteria)
  {
    motor1_PulseWidth = pulseWidthMed;
  }
  //High criteria:
  else
  {
    motor1_PulseWidth = pulseWidthHigh;
  }

  //Motor 2:
  //Low criteria:
  if(motor2_stepsPerCycle <= lowCriteria)
  {
    motor2_PulseWidth = pulseWidthLow;
  }
  //Medium criteria:
  else if(motor2_stepsPerCycle <= medCriteria)
  {
    motor2_PulseWidth = pulseWidthMed;
  }
  //High criteria:
  else
  {
    motor2_PulseWidth = pulseWidthHigh;
  }

  //Motor 3:
  //Low criteria:
  if(motor3_stepsPerCycle <= lowCriteria)
  {
    motor3_PulseWidth = pulseWidthLow;
  }
  //Medium criteria:
  else if(motor3_stepsPerCycle <= medCriteria)
  {
    motor3_PulseWidth = pulseWidthMed;
  }
  //High criteria:
  else
  {
    motor3_PulseWidth = pulseWidthHigh;
  }
}

void setState(byte motorNumber, byte stateType) {
  //Contains all state mapping data for motors 0-3.
  //stateType = 0: Motor is off.
  //stateType = 1: Forward step 1 (reverse step 4)
  //StateType = 2: Forward step 2 (reverse step 3)
  //StateType = 3: Forward step 3 (reverse step 2)
  //StateType = 4: Forward step 4 (reverse step 1)
  
  //Determine which motor is being updated and update with correct state type:
  if(motorNumber == 0)
  {
    switch(stateType)
    {
      case 0:
        digitalWrite(pin0, LOW);
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        break;
      case 1:
        digitalWrite(pin0, HIGH);
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        break;
      case 2:
        digitalWrite(pin0, LOW);
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        break;
      case 3:
        digitalWrite(pin0, LOW);
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        break;
      case 4:
        digitalWrite(pin0, HIGH);
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        break;
      default:
        break;
    }
  }
  if(motorNumber == 1)
  {
    switch(stateType)
    {
      case 0:
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, LOW);
        digitalWrite(pin6, LOW);
        digitalWrite(pin7, LOW);
        break;
      case 1:
        digitalWrite(pin4, HIGH);
        digitalWrite(pin5, LOW);
        digitalWrite(pin6, HIGH);
        digitalWrite(pin7, LOW);
        break;
      case 2:
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, HIGH);
        digitalWrite(pin6, HIGH);
        digitalWrite(pin7, LOW);
        break;
      case 3:
        digitalWrite(pin4, LOW);
        digitalWrite(pin5, HIGH);
        digitalWrite(pin6, LOW);
        digitalWrite(pin7, HIGH);
        break;
      case 4:
        digitalWrite(pin4, HIGH);
        digitalWrite(pin5, LOW);
        digitalWrite(pin6, LOW);
        digitalWrite(pin7, HIGH);
        break;
      default:
        break;
    }
  }
  if(motorNumber == 2)
  {
    switch(stateType)
    {
      case 0:
        digitalWrite(pin8, LOW);
        digitalWrite(pin9, LOW);
        digitalWrite(pin10, LOW);
        digitalWrite(pin11, LOW);
        break;
      case 1:
        digitalWrite(pin8, HIGH);
        digitalWrite(pin9, LOW);
        digitalWrite(pin10, HIGH);
        digitalWrite(pin11, LOW);
        break;
      case 2:
        digitalWrite(pin8, LOW);
        digitalWrite(pin9, HIGH);
        digitalWrite(pin10, HIGH);
        digitalWrite(pin11, LOW);
        break;
      case 3:
        digitalWrite(pin8, LOW);
        digitalWrite(pin9, HIGH);
        digitalWrite(pin10, LOW);
        digitalWrite(pin11, HIGH);
        break;
      case 4:
        digitalWrite(pin8, HIGH);
        digitalWrite(pin9, LOW);
        digitalWrite(pin10, LOW);
        digitalWrite(pin11, HIGH);
        break;
      default:
        break;
    }
  }
  if(motorNumber == 3)
  {
    switch(stateType)
    {
      case 0:
        digitalWrite(pin12, LOW);
        digitalWrite(pin13, LOW);
        digitalWrite(pin14, LOW);
        digitalWrite(pin15, LOW);
        break;
      case 1:
        digitalWrite(pin12, HIGH);
        digitalWrite(pin13, LOW);
        digitalWrite(pin14, HIGH);
        digitalWrite(pin15, LOW);
        break;
      case 2:
        digitalWrite(pin12, LOW);
        digitalWrite(pin13, HIGH);
        digitalWrite(pin14, HIGH);
        digitalWrite(pin15, LOW);
        break;
      case 3:
        digitalWrite(pin12, LOW);
        digitalWrite(pin13, HIGH);
        digitalWrite(pin14, LOW);
        digitalWrite(pin15, HIGH);
        break;
      case 4:
        digitalWrite(pin12, HIGH);
        digitalWrite(pin13, LOW);
        digitalWrite(pin14, LOW);
        digitalWrite(pin15, HIGH);
        break;
      default:
        break;
    }
  }
  
}

bool checkStepTiming(int motorNum, int stepType) {  //Return TRUE if it is OK to step the motor.
  //Values for stepType:
  //0 = No Delay
  //1 = Full Step
  //2 = (Full Step) - (Pulse Width)
  //3 = Pulse Width (of specified motor)

  if(motorNum == 0)
  {
    //Evaluate elapsed time since last update:
    motor0_elapsed = micros() - motor0_lastMicros;

    //Choose the appropriate step duration:
    switch(stepType)
    {
      case 0:
        //Reset timing:
        motor0_lastMicros = micros();
        return(1);
        break;
      case 1:
      if(motor0_elapsed >= (timeForSteps/((unsigned long)motor0_stepsPerCycle)))
            {
              //Reset timing:
              motor0_lastMicros = micros();
              return(1);
            }
        break;
      case 2:
      if(motor0_elapsed >= (((timeForSteps/((unsigned long)motor0_stepsPerCycle))-motor0_PulseWidth)))
            {
              //Reset timing:
              motor0_lastMicros = micros();
              return(1);
            }
        break;
      case 3:
      if(motor0_elapsed >= motor0_PulseWidth)
            {
              //Reset timing:
              motor0_lastMicros = micros();
              return(1);
            }
        break;
      default:
        return(0);
    }
    return(0);
  }
  if(motorNum == 1)
  {
    //Evaluate elapsed time since last update:
    motor1_elapsed = micros() - motor1_lastMicros;

    //Choose the appropriate step duration:
    switch(stepType)
    {
      case 0:
        //Reset timing:
        motor1_lastMicros = micros();
        return(1);
        break;
      case 1:
      if(motor1_elapsed >= (timeForSteps/((unsigned long)motor1_stepsPerCycle)))
            {
              //Reset timing:
              motor1_lastMicros = micros();
              return(1);
            }
        break;
      case 2:
      if(motor1_elapsed >= (((timeForSteps/((unsigned long)motor1_stepsPerCycle))-motor1_PulseWidth)))
            {
              //Reset timing:
              motor1_lastMicros = micros();
              return(1);
            }
        break;
      case 3:
      if(motor1_elapsed >= motor1_PulseWidth)
            {
              //Reset timing:
              motor1_lastMicros = micros();
              return(1);
            }
        break;
      default:
        return(0);
    }
    return(0);
  }
  if(motorNum == 2)
  {
    //Evaluate elapsed time since last update:
    motor2_elapsed = micros() - motor2_lastMicros;

    //Choose the appropriate step duration:
    switch(stepType)
    {
      case 0:
        //Reset timing:
        motor2_lastMicros = micros();
        return(1);
        break;
      case 1:
      if(motor2_elapsed >= (timeForSteps/((unsigned long)motor2_stepsPerCycle)))
            {
              //Reset timing:
              motor2_lastMicros = micros();
              return(1);
            }
        break;
      case 2:
      if(motor2_elapsed >= (((timeForSteps/((unsigned long)motor2_stepsPerCycle))-motor2_PulseWidth)))
            {
              //Reset timing:
              motor2_lastMicros = micros();
              return(1);
            }
        break;
      case 3:
      if(motor2_elapsed >= motor2_PulseWidth)
            {
              //Reset timing:
              motor2_lastMicros = micros();
              return(1);
            }
        break;
      default:
        return(0);
    }
    return(0);
  }
  if(motorNum == 3)
  {
    //Evaluate elapsed time since last update:
    motor3_elapsed = micros() - motor3_lastMicros;

    //Choose the appropriate step duration:
    switch(stepType)
    {
      case 0:
        //Reset timing:
        motor3_lastMicros = micros();
        return(1);
        break;
      case 1:
      if(motor3_elapsed >= (timeForSteps/((unsigned long)motor3_stepsPerCycle)))
            {
              //Reset timing:
              motor3_lastMicros = micros();
              return(1);
            }
        break;
      case 2:
      if(motor3_elapsed >= (((timeForSteps/((unsigned long)motor3_stepsPerCycle))-motor3_PulseWidth)))
            {
              //Reset timing:
              motor3_lastMicros = micros();
              return(1);
            }
        break;
      case 3:
      if(motor3_elapsed >= motor3_PulseWidth)
            {
              //Reset timing:
              motor3_lastMicros = micros();
              return(1);
            }
        break;
      default:
        return(0);
    }
    return(0);
  }
}

void motor0_Update() {    //Update motor0:
  //If steps value is 0, turn off motor:
  if(motor0_stepsPerCycle == 0)
  {
    //Set motor state to 0 (off):
    setState(0, 0);
  }
  else
  {
    //Determine if pulse-driving is specified:
    if(!motor0_UsePulse)  //No pulse-driving
    {   
      //Determine direction of motor:
      if(motor0_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor0_CurrentStep)
        {
          case 0:
            if(checkStepTiming(0, 1))
            {
              setState(0, 1);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(0, 1))
            {
              setState(0, 2);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(0, 1))
            {
              setState(0, 3);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(0, 1))
            {
              setState(0, 4);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
      else //Reverse
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor0_CurrentStep)
        {
          case 0:
            if(checkStepTiming(0, 1))
            {
              setState(0, 4);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(0, 1))
            {
              setState(0, 3);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(0, 1))
            {
              setState(0, 2);
              //Increment current step value:
              motor0_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(0, 1))
            {
              setState(0, 1);
            //Increment current step value:
            motor0_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
  
        //Determine if steps are complete:
        if(motor0_CurrentStep >= 4)
        {
          //Reset current step value:
          motor0_CurrentStep = 0;
        }     
    }
    else  //Pulse-driving
    {      
      //Determine direction of motor:
      if(motor0_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor0_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(0, 2))
            {
              setState(0, 1);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(0, 2))
            {
              setState(0, 2);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(0, 2))
            {
              setState(0, 3);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(0, 2))
            {
              setState(0, 4);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }
      else  //Reverse
      {
        //Write motor state to GPIO:
        switch(motor0_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(0, 2))
            {
              setState(0, 4);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(0, 2))
            {
              setState(0, 3);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(0, 2))
            {
              setState(0, 2);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(0, 2))
            {
              setState(0, 1);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(0, 3))
            {
              setState(0, 0);
              //Increment current step value:
              motor0_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }

        //Determine if steps are complete:
        if(motor0_CurrentPulseStep >= 8)
        {
          //Reset current step value:
          motor0_CurrentPulseStep = 0;
        }
    }
  }
}

void motor1_Update() {    //Update motor1:
  //If steps value is 0, turn off motor:
  if(motor1_stepsPerCycle == 0)
  {
    //Set motor state to 0 (off):
    setState(1, 0);
  }
  else
  {
    //Determine if pulse-driving is specified:
    if(!motor1_UsePulse)  //No pulse-driving
    {   
      //Determine direction of motor:
      if(motor1_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor1_CurrentStep)
        {
          case 0:
            if(checkStepTiming(1, 1))
            {
              setState(1, 1);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(1, 1))
            {
              setState(1, 2);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(1, 1))
            {
              setState(1, 3);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(1, 1))
            {
              setState(1, 4);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
      else //Reverse
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor1_CurrentStep)
        {
          case 0:
            if(checkStepTiming(1, 1))
            {
              setState(1, 4);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(1, 1))
            {
              setState(1, 3);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(1, 1))
            {
              setState(1, 2);
              //Increment current step value:
              motor1_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(1, 1))
            {
              setState(1, 1);
            //Increment current step value:
            motor1_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
  
        //Determine if steps are complete:
        if(motor1_CurrentStep >= 4)
        {
          //Reset current step value:
          motor1_CurrentStep = 0;
        }     
    }
    else  //Pulse-driving
    {      
      //Determine direction of motor:
      if(motor1_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor1_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(1, 2))
            {
              setState(1, 1);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(1, 2))
            {
              setState(1, 2);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(1, 2))
            {
              setState(1, 3);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(1, 2))
            {
              setState(1, 4);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }
      else  //Reverse
      {
        //Write motor state to GPIO:
        switch(motor1_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(1, 2))
            {
              setState(1, 4);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(1, 2))
            {
              setState(1, 3);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(1, 2))
            {
              setState(1, 2);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(1, 2))
            {
              setState(1, 1);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(1, 3))
            {
              setState(1, 0);
              //Increment current step value:
              motor1_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }

        //Determine if steps are complete:
        if(motor1_CurrentPulseStep >= 8)
        {
          //Reset current step value:
          motor1_CurrentPulseStep = 0;
        }
    }
  }
}

void motor2_Update() {    //Update motor2:
  //If steps value is 0, turn off motor:
  if(motor2_stepsPerCycle == 0)
  {
    //Set motor state to 0 (off):
    setState(2, 0);
  }
  else
  {
    //Determine if pulse-driving is specified:
    if(!motor2_UsePulse)  //No pulse-driving
    {   
      //Determine direction of motor:
      if(motor2_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor2_CurrentStep)
        {
          case 0:
            if(checkStepTiming(2, 1))
            {
              setState(2, 1);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(2, 1))
            {
              setState(2, 2);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(2, 1))
            {
              setState(2, 3);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(2, 1))
            {
              setState(2, 4);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
      else //Reverse
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor2_CurrentStep)
        {
          case 0:
            if(checkStepTiming(2, 1))
            {
              setState(2, 4);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(2, 1))
            {
              setState(2, 3);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(2, 1))
            {
              setState(2, 2);
              //Increment current step value:
              motor2_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(2, 1))
            {
              setState(2, 1);
            //Increment current step value:
            motor2_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
  
        //Determine if steps are complete:
        if(motor2_CurrentStep >= 4)
        {
          //Reset current step value:
          motor2_CurrentStep = 0;
        }     
    }
    else  //Pulse-driving
    {      
      //Determine direction of motor:
      if(motor2_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor2_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(2, 2))
            {
              setState(2, 1);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(2, 2))
            {
              setState(2, 2);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(2, 2))
            {
              setState(2, 3);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(2, 2))
            {
              setState(2, 4);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }
      else  //Reverse
      {
        //Write motor state to GPIO:
        switch(motor2_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(2, 2))
            {
              setState(2, 4);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(2, 2))
            {
              setState(2, 3);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(2, 2))
            {
              setState(2, 2);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(2, 2))
            {
              setState(2, 1);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(2, 3))
            {
              setState(2, 0);
              //Increment current step value:
              motor2_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }

        //Determine if steps are complete:
        if(motor2_CurrentPulseStep >= 8)
        {
          //Reset current step value:
          motor2_CurrentPulseStep = 0;
        }
    }
  }
}

void motor3_Update() {    //Update motor3:
  //If steps value is 0, turn off motor:
  if(motor3_stepsPerCycle == 0)
  {
    //Set motor state to 0 (off):
    setState(3, 0);
  }
  else
  {
    //Determine if pulse-driving is specified:
    if(!motor3_UsePulse)  //No pulse-driving
    {   
      //Determine direction of motor:
      if(motor3_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor3_CurrentStep)
        {
          case 0:
            if(checkStepTiming(3, 1))
            {
              setState(3, 1);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(3, 1))
            {
              setState(3, 2);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(3, 1))
            {
              setState(3, 3);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(3, 1))
            {
              setState(3, 4);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
      else //Reverse
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor3_CurrentStep)
        {
          case 0:
            if(checkStepTiming(3, 1))
            {
              setState(3, 4);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 1:
            if(checkStepTiming(3, 1))
            {
              setState(3, 3);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 2:
            if(checkStepTiming(3, 1))
            {
              setState(3, 2);
              //Increment current step value:
              motor3_CurrentStep++;
            }
            break;
          case 3:
            if(checkStepTiming(3, 1))
            {
              setState(3, 1);
            //Increment current step value:
            motor3_CurrentStep++;
            }
            break;
          default:
            break;
        }
      }
  
        //Determine if steps are complete:
        if(motor3_CurrentStep >= 4)
        {
          //Reset current step value:
          motor3_CurrentStep = 0;
        }     
    }
    else  //Pulse-driving
    {      
      //Determine direction of motor:
      if(motor3_Direction)  //Forward
      {
        //Write motor state to GPIO (if time is correct):
        switch(motor3_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(3, 2))
            {
              setState(3, 1);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(3, 2))
            {
              setState(3, 2);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(3, 2))
            {
              setState(3, 3);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(3, 2))
            {
              setState(3, 4);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }
      else  //Reverse
      {
        //Write motor state to GPIO:
        switch(motor3_CurrentPulseStep)
        {
          case 0:
            if(checkStepTiming(3, 2))
            {
              setState(3, 4);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 1:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 2:
            if(checkStepTiming(3, 2))
            {
              setState(3, 3);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 3:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 4:
            if(checkStepTiming(3, 2))
            {
              setState(3, 2);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 5:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 6:
            if(checkStepTiming(3, 2))
            {
              setState(3, 1);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          case 7:
            if(checkStepTiming(3, 3))
            {
              setState(3, 0);
              //Increment current step value:
              motor3_CurrentPulseStep++;
            }
            break;
          default:
            break;
        }
      }

        //Determine if steps are complete:
        if(motor3_CurrentPulseStep >= 8)
        {
          //Reset current step value:
          motor3_CurrentPulseStep = 0;
        }
    }
  }
}

void loop() { //Loop code:
  //Determine if data is in the serial buffer and store it in the data buffer:
  checkSerial();
  
  //Determine if the mid-packet timeout has been reached:
  checkPacketTimeout();

  //Determine if the buffer reading cycle (default 100ms) has passed:
  if(cycleTimer())
  {
     //Perform buffer read if there is data in the buffer (motor0):
     if(buffer0ReadIndex != buffer0WriteIndex)
     {
       //Read and apply the contents of buffer 0 at the appropriate index:
       readBuffer0();
     }
     else
     {
       //Otherwise, set steps to 0 to turn off motor:
       motor0_stepsPerCycle = 0;
     }

     //Perform buffer read if there is data in the buffer (motor1):
     if(buffer1ReadIndex != buffer1WriteIndex)
     {
       //Read and apply the contents of buffer 1 at the appropriate index:
       readBuffer1();
     }
     else
     {
       //Otherwise, set steps to 0 to turn off motor:
       motor1_stepsPerCycle = 0;
     }

     //Perform buffer read if there is data in the buffer (motor2):
     if(buffer2ReadIndex != buffer2WriteIndex)
     {
       //Read and apply the contents of buffer 2 at the appropriate index:
       readBuffer2();
     }
     else
     {
       //Otherwise, set steps to 0 to turn off motor:
       motor2_stepsPerCycle = 0;
     }

     //Perform buffer read if there is data in the buffer (motor3):
     if(buffer3ReadIndex != buffer3WriteIndex)
     {
       //Read and apply the contents of buffer 3 at the appropriate index:
       readBuffer3();
     }
     else
     {
       //Otherwise, set steps to 0 to turn off motor:
       motor3_stepsPerCycle = 0;
     }
  }

  //Determine if motors should be operating in pulse-drive mode:
  checkPulseDrives();

  //Determine pulse width for pulse-drive mode:
  setPulseWidths();

  //Perform updates to motor states (if applicable):
  motor0_Update();
  motor1_Update();
  motor2_Update();
  motor3_Update();
  
}
