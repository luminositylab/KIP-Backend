//Loop-based stepper motor driver for MYRA/KIP robot
//Buffered version
//(C) 2018 The Luminosity Lab

//This program controls the H-bridges for 4 stepper motors.
//It utilizes a fixed-frequency step-updating loop.
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
const int numberOfBytes = 6;          //Number of bytes to expect per packet
const byte usePacketBit = 0;          //Specifies which bit in Byte 0 is 'use packet'
const byte usePulseBit = 1;           //Specifies which bit in Byte 0 is 'use pulse-driving'
const byte directionBit = 2;          //Specifies which bit in Byte 0 is 'direction'
const byte motorNumber0Bit = 3;       //Specifies which bit in Byte 0 is first to indicate which motor is being controlled
const byte motorNumber1Bit = 4;       //Specifies which bit in Byte 0 is second to indicate which motor is being controlled
const byte forcePacketBit = 5;        //Specifies which bit in Byte 0 forces application of packet contents
const bool activeHigh = 0;            //Set 0 to utilize packet when 'use packet' bit is 0
const long baudRate = 250000;         //Specifies serial baud rate
const byte pulseCriteria = 10;        //Specifies minimum divider value for which to enable pulse-driving
const byte pulseWidthLow = 20;        //Specifies the width of pulse-driving pulses for low speeds (in loop iterations)
const byte pulseWidthMed = 10;        //Specifies the width of pulse-driving pulses for medium speeds (in loop iterations)
const byte pulseWidthHigh = 5;        //Specifies the width of pulse-driving pulses for high speeds (in loop iterations)
const byte lowCriteria = 60;          //Specifies divider value for low speed
const byte medCriteria = 30;          //Specifies divider value for medium speed
const byte highCriteria = 10;         //Specifies divider value for high speed
const long compensatedDelay0 = 290;   //Specifies compensated program delay in (us) for ~2 kHz loop rate
const long compensatedDelay1 = 25;    //Specifies execution delay compensation in (us)
const long compensatedDelay2 = 80;    //Specifies buffer write delay compensation in (us)
const int bufferSize = 250;           //Size of data buffer
const int timeForSteps = 100;         //Time in (ms) for each set of steps to be executed.

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
const byte resetPacketPin = 18;

//Declaration of program variables:
byte receivedByte = 0;              //Number of the byte received from the packet
byte dataContents = 0;              //Contents of the most recent serial byte
bool usePacket = 0;                 //'Use Packet' bit
bool usePulseDrive = 0;             //'Use Pulse-Driving' bit
bool directionData = 0;             //'Direction' bit
bool forcePacket = 0;               //'Force packet application' bit
byte motor0_Divider = 255;          //Input divider (0 = fastest, 254 = slowest, 255 = off) for motor0
byte motor1_Divider = 255;          //Input divider (0 = fastest, 254 = slowest, 255 = off) for motor1
byte motor2_Divider = 255;          //Input divider (0 = fastest, 254 = slowest, 255 = off) for motor2
byte motor3_Divider = 255;          //Input divider (0 = fastest, 254 = slowest, 255 = off) for motor3
int motor0_Counter = 0;             //Loop counter for motor0
int motor1_Counter = 0;             //Loop counter for motor1
int motor2_Counter = 0;             //Loop counter for motor2
int motor3_Counter = 0;             //Loop counter for motor3
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
bool motor0_MidStep = 0;            //Indicates whether mid-step has been updated for motor0
bool motor1_MidStep = 0;            //Indicates whether mid-step has been updated for motor1
bool motor2_MidStep = 0;            //Indicates whether mid-step has been updated for motor2
bool motor3_MidStep = 0;            //Indicates whether mid-step has been updated for motor3
byte motor0_CurrentStep = 0;        //Current step for motor0
byte motor1_CurrentStep = 0;        //Current step for motor1
byte motor2_CurrentStep = 0;        //Current step for motor2
byte motor3_CurrentStep = 0;        //Current step for motor3
byte motor0_CurrentPulseStep = 0;   //Current pulse-driving step for motor0
byte motor1_CurrentPulseStep = 0;   //Current pulse-driving step for motor1
byte motor2_CurrentPulseStep = 0;   //Current pulse-driving step for motor2
byte motor3_CurrentPulseStep = 0;   //Current pulse-driving step for motor3
byte motor0_PulseWidth = 5;         //Specifies the width of pulse-driving pulses (in loop iterations) for motor0
byte motor1_PulseWidth = 5;         //Specifies the width of pulse-driving pulses (in loop iterations) for motor1
byte motor2_PulseWidth = 5;         //Specifies the width of pulse-driving pulses (in loop iterations) for motor2
byte motor3_PulseWidth = 5;         //Specifies the width of pulse-driving pulses (in loop iterations) for motor3
byte packetMotorNumber = 0;         //Specifies the motor number to which the packet applies.
int buffer0WriteIndex = 0;          //Write index for motor 0 buffers
int buffer1WriteIndex = 0;          //Write index for motor 1 buffers
int buffer2WriteIndex = 0;          //Write index for motor 2 buffers
int buffer3WriteIndex = 0;          //Write index for motor 3 buffers
int buffer0ReadIndex = 0;           //Read index for motor 0 buffers
int buffer1ReadIndex = 0;           //Read index for motor 1 buffers
int buffer2ReadIndex = 0;           //Read index for motor 2 buffers
int buffer3ReadIndex = 0;           //Read index for motor 3 buffers
int cycleCount = 0;                 //Cycle counter

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
  pinMode(resetPacketPin, INPUT);

}

void checkPacketReset() {           //Reset packet counter if resetPacketPin has been pulled high
  if(digitalRead(resetPacketPin))
  {
    receivedByte = 0;
  }
}

bool cycleCounter() {        //Track cycles performed and return TRUE if goal is met.
  //Determine if timeForSteps has passed (assuming 1 cycle ~ 0.5 ms)
  if(cycleCount > 2*timeForSteps)
  {
    //Reset cycle counter:
    cycleCount = 0;
    //Return TRUE:
    return 1;
  }
  //Otherwise, increment cycle counter and return FALSE:
  cycleCount++;
  return 0;
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
  //If the data contains a 0-value for steps, set divider to 255 to indicate STOP:
  if(stepBuffer0[buffer0ReadIndex] == 0)
  {
    motor0_Divider = 255;
  }
  
  //Determine the cycle divider value based on the step time:
  else
  {
    motor0_Divider = timeForSteps/stepBuffer0[buffer0ReadIndex];
  }
  
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
  //If the data contains a 0-value for steps, set divider to 255 to indicate STOP:
  if(stepBuffer1[buffer1ReadIndex] == 0)
  {
    motor1_Divider = 255;
  }
  
  //Determine the cycle divider value based on the step time:
  else
  {
    motor1_Divider = timeForSteps/stepBuffer1[buffer1ReadIndex];
  }
  
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
  
  //If the data contains a 0-value for steps, set divider to 255 to indicate STOP:
  if(stepBuffer2[buffer2ReadIndex] == 0)
  {
    motor2_Divider = 255;
  }
  
  //Determine the cycle divider value based on the step time:
  else
  {
    motor2_Divider = timeForSteps/stepBuffer2[buffer2ReadIndex];
  }
  
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
  
  //If the data contains a 0-value for steps, set divider to 255 to indicate STOP:
  if(stepBuffer3[buffer3ReadIndex] == 0)
  {
    motor3_Divider = 255;
  }
  
  //Determine the cycle divider value based on the step time:
  else
  {
    motor3_Divider = timeForSteps/stepBuffer3[buffer3ReadIndex];
  }
  
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

void checkSerial() {                //Determine if data is in the serial buffer and store it in the data buffer:
  checkPacketReset();               //Reset packet counter if resetPacketPin has been pulled high

  if(Serial.available() > 0)
  {
    while(Serial.available() > 0)
    {
      dataContents = Serial.read();   //Load most recent byte into dataContents
      
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
  else
  {
    //Compensate for delay introduced by the serial data acquisition:
    delayMicroseconds(compensatedDelay2);
  }
}

void checkPulseDrive() {          //Determine if pulse-driving should be used for each motor:
    if(motor0_UsePulse_Data && (motor0_Divider >= pulseCriteria))
    {
      motor0_UsePulse = 1;
    }
    else
    {
      motor0_UsePulse = 0;
    }
    if(motor1_UsePulse_Data && (motor1_Divider >= pulseCriteria))
    {
      motor1_UsePulse = 1;
    }
    else
    {
      motor1_UsePulse = 0;
    }
    if(motor2_UsePulse_Data && (motor2_Divider >= pulseCriteria))
    {
      motor2_UsePulse = 1;
    }
    else
    {
      motor2_UsePulse = 0;
    }
    if(motor3_UsePulse_Data && (motor3_Divider >= pulseCriteria))
    {
      motor3_UsePulse = 1;
    }
    else
    {
      motor3_UsePulse = 0;
    }
  
}

void getPulseWidth() {          //Determine pulse width for pulse-drive mode:
  //Motor 0:
  if(motor0_Divider >= lowCriteria)
  {
    motor0_PulseWidth = pulseWidthLow;
  }
  else if(motor0_Divider >= medCriteria)
  {
    motor0_PulseWidth = pulseWidthMed;
  }
  else
  {
    motor0_PulseWidth = pulseWidthHigh;
  }
  //Motor 1:
  if(motor1_Divider >= lowCriteria)
  {
    motor1_PulseWidth = pulseWidthLow;
  }
  else if(motor1_Divider >= medCriteria)
  {
    motor1_PulseWidth = pulseWidthMed;
  }
  else
  {
    motor1_PulseWidth = pulseWidthHigh;
  }
  //Motor 2:
  if(motor2_Divider >= lowCriteria)
  {
    motor2_PulseWidth = pulseWidthLow;
  }
  else if(motor2_Divider >= medCriteria)
  {
    motor2_PulseWidth = pulseWidthMed;
  }
  else
  {
    motor2_PulseWidth = pulseWidthHigh;
  }
  //Motor 3:
  if(motor3_Divider >= lowCriteria)
  {
    motor3_PulseWidth = pulseWidthLow;
  }
  else if(motor3_Divider >= medCriteria)
  {
    motor3_PulseWidth = pulseWidthMed;
  }
  else
  {
    motor3_PulseWidth = pulseWidthHigh;
  }
}

bool motor0_CheckUpdate() {       //Determine if motor0 should be updated:
  //If pulse-driving is enabled, report mid-step pulse updates:
  if(motor0_UsePulse)
  {
    if((!motor0_MidStep) && (motor0_Counter >= ((motor0_Divider-motor0_PulseWidth))))
    {
       motor0_MidStep = 1;   //Flag that a mid-step update has been triggered
       return(1);            //Report that update is required
    }
    if(motor0_MidStep && (motor0_Counter >= ((motor0_Divider-motor0_PulseWidth)+motor0_PulseWidth)))
    {
       motor0_MidStep = 0;   //Flag that a mid-step update has not been triggered
       motor0_Counter = 0;   //Reset cycle counter
       return(1);            //Report that update is required
    }
    motor0_Counter++;     //Increment cycle counter
    return(0);            //Report that update is not required
  }
  //Otherwise, proceed with normal timing:
  else
  {
    if(motor0_Counter >= motor0_Divider)
    {
      motor0_Counter = 0;   //Reset cycle counter
      return(1);            //Report that update is required
    }
    else
    {
      motor0_Counter++;     //Increment cycle counter
      return(0);            //Report that update is not required
    }
  }
}

bool motor1_CheckUpdate() {       //Determine if motor1 should be updated:
  //If pulse-driving is enabled, report mid-step pulse updates:
  if(motor1_UsePulse)
  {
    if((!motor1_MidStep) && (motor1_Counter >= ((motor1_Divider-motor1_PulseWidth))))
    {
       motor1_MidStep = 1;   //Flag that a mid-step update has been triggered
       return(1);            //Report that update is required
    }
    if(motor1_MidStep && (motor1_Counter >= ((motor1_Divider-motor1_PulseWidth)+motor1_PulseWidth)))
    {
       motor1_MidStep = 0;   //Flag that a mid-step update has not been triggered
       motor1_Counter = 0;   //Reset cycle counter
       return(1);            //Report that update is required
    }
    motor1_Counter++;     //Increment cycle counter
    return(0);            //Report that update is not required
  }
  //Otherwise, proceed with normal timing:
  else
  {
    if(motor1_Counter >= motor1_Divider)
    {
      motor1_Counter = 0;   //Reset cycle counter
      return(1);            //Report that update is required
    }
    else
    {
      motor1_Counter++;     //Increment cycle counter
      return(0);            //Report that update is not required
    }
  }
}

bool motor2_CheckUpdate() {       //Determine if motor2 should be updated:
  //If pulse-driving is enabled, report mid-step pulse updates:
  if(motor2_UsePulse)
  {
    if((!motor2_MidStep) && (motor2_Counter >= ((motor2_Divider-motor2_PulseWidth))))
    {
       motor2_MidStep = 1;   //Flag that a mid-step update has been triggered
       return(1);            //Report that update is required
    }
    if(motor2_MidStep && (motor2_Counter >= ((motor2_Divider-motor2_PulseWidth)+motor2_PulseWidth)))
    {
       motor2_MidStep = 0;   //Flag that a mid-step update has not been triggered
       motor2_Counter = 0;   //Reset cycle counter
       return(1);            //Report that update is required
    }
    motor2_Counter++;     //Increment cycle counter
    return(0);            //Report that update is not required
  }
  //Otherwise, proceed with normal timing:
  else
  {
    if(motor2_Counter >= motor2_Divider)
    {
      motor2_Counter = 0;   //Reset cycle counter
      return(1);            //Report that update is required
    }
    else
    {
      motor2_Counter++;     //Increment cycle counter
      return(0);            //Report that update is not required
    }
  }
}

bool motor3_CheckUpdate() {       //Determine if motor3 should be updated:
  //If pulse-driving is enabled, report mid-step pulse updates:
  if(motor3_UsePulse)
  {
    if((!motor3_MidStep) && (motor3_Counter >= ((motor3_Divider-motor3_PulseWidth))))
    {
       motor3_MidStep = 1;   //Flag that a mid-step update has been triggered
       return(1);            //Report that update is required
    }
    if(motor3_MidStep && (motor3_Counter >= ((motor3_Divider-motor3_PulseWidth)+motor3_PulseWidth)))
    {
       motor3_MidStep = 0;   //Flag that a mid-step update has not been triggered
       motor3_Counter = 0;   //Reset cycle counter
       return(1);            //Report that update is required
    }
    motor3_Counter++;     //Increment cycle counter
    return(0);            //Report that update is not required
  }
  //Otherwise, proceed with normal timing:
  else
  {
    if(motor3_Counter >= motor3_Divider)
    {
      motor3_Counter = 0;   //Reset cycle counter
      return(1);            //Report that update is required
    }
    else
    {
      motor3_Counter++;     //Increment cycle counter
      return(0);            //Report that update is not required
    }
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

void motor0_Update() {    //Update motor0 without pulse-stepping
  if(motor0_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(0, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor0_Direction)            //Determine direction of motor
    {
      switch(motor0_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(0, 1);
          break;
        case 1:
          setState(0, 2);
          break;
        case 2:
          setState(0, 3);
          break;
        case 3:
          setState(0, 4);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor0_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(0, 4);
          break;
        case 1:
          setState(0, 3);
          break;
        case 2:
          setState(0, 2);
          break;
        case 3:
          setState(0, 1);
          break;
        default:
          break;
      }
    }
  }

  motor0_CurrentStep++;           //Increment current step value
  if(motor0_CurrentStep >= 4)     //Determine if steps are complete
  {
    motor0_CurrentStep = 0;       //Reset current step value
  }
}

void motor0_UpdatePulse() {    //Update motor0 with pulse-stepping
  if(motor0_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(0, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor0_Direction)            //Determine direction of motor
    {
      switch(motor0_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(0, 1);
          break;
        case 1:
          setState(0, 0);
          break;
        case 2:
          setState(0, 2);
          break;
        case 3:
          setState(0, 0);
          break;
        case 4:
          setState(0, 3);
          break;
        case 5:
          setState(0, 0);
          break;
        case 6:
          setState(0, 4);
          break;
        case 7:
          setState(0, 0);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor0_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(0, 4);
          break;
        case 1:
          setState(0, 0);
          break;
        case 2:
          setState(0, 3);
          break;
        case 3:
          setState(0, 0);
          break;
        case 4:
          setState(0, 2);
          break;
        case 5:
          setState(0, 0);
          break;
        case 6:
          setState(0, 1);
          break;
        case 7:
          setState(0, 0);
          break;
        default:
          break;
      }
    }
  }

  motor0_CurrentPulseStep++;            //Increment current step value
  if(motor0_CurrentPulseStep >= 8)      //Determine if steps are complete
  {
    motor0_CurrentPulseStep = 0;        //Reset current step value
  }
}

void motor1_Update() {    //Update motor1 without pulse-stepping
  if(motor1_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(1, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor1_Direction)            //Determine direction of motor
    {
      switch(motor1_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(1, 1);
          break;
        case 1:
          setState(1, 2);
          break;
        case 2:
          setState(1, 3);
          break;
        case 3:
          setState(1, 4);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor1_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(1, 4);
          break;
        case 1:
          setState(1, 3);
          break;
        case 2:
          setState(1, 2);
          break;
        case 3:
          setState(1, 1);
          break;
        default:
          break;
      }
    }
  }

  motor1_CurrentStep++;           //Increment current step value
  if(motor1_CurrentStep >= 4)     //Determine if steps are complete
  {
    motor1_CurrentStep = 0;       //Reset current step value
  }
}

void motor1_UpdatePulse() {    //Update motor1 with pulse-stepping
  if(motor1_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(1, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor1_Direction)            //Determine direction of motor
    {
      switch(motor1_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(1, 1);
          break;
        case 1:
          setState(1, 0);
          break;
        case 2:
          setState(1, 2);
          break;
        case 3:
          setState(1, 0);
          break;
        case 4:
          setState(1, 3);
          break;
        case 5:
          setState(1, 0);
          break;
        case 6:
          setState(1, 4);
          break;
        case 7:
          setState(1, 0);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor1_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(1, 4);
          break;
        case 1:
          setState(1, 0);
          break;
        case 2:
          setState(1, 3);
          break;
        case 3:
          setState(1, 0);
          break;
        case 4:
          setState(1, 2);
          break;
        case 5:
          setState(1, 0);
          break;
        case 6:
          setState(1, 1);
          break;
        case 7:
          setState(1, 0);
          break;
        default:
          break;
      }
    }
  }

  motor1_CurrentPulseStep++;            //Increment current step value
  if(motor1_CurrentPulseStep >= 8)      //Determine if steps are complete
  {
    motor1_CurrentPulseStep = 0;        //Reset current step value
  }
}

void motor2_Update() {    //Update motor2 without pulse-stepping
  if(motor2_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(2, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor2_Direction)            //Determine direction of motor
    {
      switch(motor2_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(2, 1);
          break;
        case 1:
          setState(2, 2);
          break;
        case 2:
          setState(2, 3);
          break;
        case 3:
          setState(2, 4);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor2_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(2, 4);
          break;
        case 1:
          setState(2, 3);
          break;
        case 2:
          setState(2, 2);
          break;
        case 3:
          setState(2, 1);
          break;
        default:
          break;
      }
    }
  }

  motor2_CurrentStep++;           //Increment current step value
  if(motor2_CurrentStep >= 4)     //Determine if steps are complete
  {
    motor2_CurrentStep = 0;       //Reset current step value
  }
}

void motor2_UpdatePulse() {    //Update motor2 with pulse-stepping
  if(motor2_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(2, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor2_Direction)            //Determine direction of motor
    {
      switch(motor2_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(2, 1);
          break;
        case 1:
          setState(2, 0);
          break;
        case 2:
          setState(2, 2);
          break;
        case 3:
          setState(2, 0);
          break;
        case 4:
          setState(2, 3);
          break;
        case 5:
          setState(2, 0);
          break;
        case 6:
          setState(2, 4);
          break;
        case 7:
          setState(2, 0);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor2_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(2, 4);
          break;
        case 1:
          setState(2, 0);
          break;
        case 2:
          setState(2, 3);
          break;
        case 3:
          setState(2, 0);
          break;
        case 4:
          setState(2, 2);
          break;
        case 5:
          setState(2, 0);
          break;
        case 6:
          setState(2, 1);
          break;
        case 7:
          setState(2, 0);
          break;
        default:
          break;
      }
    }
  }

  motor2_CurrentPulseStep++;            //Increment current step value
  if(motor2_CurrentPulseStep >= 8)      //Determine if steps are complete
  {
    motor2_CurrentPulseStep = 0;        //Reset current step value
  }
}

void motor3_Update() {    //Update motor3 without pulse-stepping
  if(motor3_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(3, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor3_Direction)            //Determine direction of motor
    {
      switch(motor3_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(3, 1);
          break;
        case 1:
          setState(3, 2);
          break;
        case 2:
          setState(3, 3);
          break;
        case 3:
          setState(3, 4);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor3_CurrentStep)    //Write motor state to GPIO
      {
        case 0:
          setState(3, 4);
          break;
        case 1:
          setState(3, 3);
          break;
        case 2:
          setState(3, 2);
          break;
        case 3:
          setState(3, 1);
          break;
        default:
          break;
      }
    }
  }

  motor3_CurrentStep++;           //Increment current step value
  if(motor3_CurrentStep >= 4)     //Determine if steps are complete
  {
    motor3_CurrentStep = 0;       //Reset current step value
  }
}

void motor3_UpdatePulse() {    //Update motor3 with pulse-stepping
  if(motor3_Divider == 255)         //If divider is 255, turn off motor
  {
    setState(3, 0);                 //Set motor state to 0 (off)
  }
  else
  {
    if(motor3_Direction)            //Determine direction of motor
    {
      switch(motor3_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(3, 1);
          break;
        case 1:
          setState(3, 0);
          break;
        case 2:
          setState(3, 2);
          break;
        case 3:
          setState(3, 0);
          break;
        case 4:
          setState(3, 3);
          break;
        case 5:
          setState(3, 0);
          break;
        case 6:
          setState(3, 4);
          break;
        case 7:
          setState(3, 0);
          break;
        default:
          break;
      }
    }
    else
    {
      switch(motor3_CurrentPulseStep)    //Write motor state to GPIO
      {
        case 0:
          setState(3, 4);
          break;
        case 1:
          setState(3, 0);
          break;
        case 2:
          setState(3, 3);
          break;
        case 3:
          setState(3, 0);
          break;
        case 4:
          setState(3, 2);
          break;
        case 5:
          setState(3, 0);
          break;
        case 6:
          setState(3, 1);
          break;
        case 7:
          setState(3, 0);
          break;
        default:
          break;
      }
    }
  }

  motor3_CurrentPulseStep++;            //Increment current step value
  if(motor3_CurrentPulseStep >= 8)      //Determine if steps are complete
  {
    motor3_CurrentPulseStep = 0;        //Reset current step value
  }
}

void loop() { //Loop code:
  checkSerial();                        //Determine if data is in the serial buffer and store it in the data buffer
  
  if(cycleCounter())                    //Track cycles performed and return TRUE if goal is met.
  {
     //Perform buffer read if there is data in the buffer:
     if(buffer0ReadIndex != buffer0WriteIndex)
     {
       readBuffer0();                   //Read and apply the contents of buffer 0 at the appropriate index
     }
     else
     {
      //Otherwise, set divider to 255 to turn off motor:
      motor0_Divider = 255;
     }
     
     //Perform buffer read if there is data in the buffer:
     if(buffer1ReadIndex != buffer1WriteIndex)
     {
       readBuffer1();                   //Read and apply the contents of buffer 1 at the appropriate index
     }
     else
     {
      //Otherwise, set divider to 255 to turn off motor:
      motor1_Divider = 255;
     }
     
     //Perform buffer read if there is data in the buffer:
     if(buffer2ReadIndex != buffer2WriteIndex)
     {
       readBuffer2();                   //Read and apply the contents of buffer 2 at the appropriate index
     }
     else
     {
      //Otherwise, set divider to 255 to turn off motor:
      motor2_Divider = 255;
     }
     
     //Perform buffer read if there is data in the buffer:
     if(buffer3ReadIndex != buffer3WriteIndex)
     {
       readBuffer3();                   //Read and apply the contents of buffer 3 at the appropriate index
     }
     else
     {
      //Otherwise, set divider to 255 to turn off motor:
      motor3_Divider = 255;
     }
  }

  checkPulseDrive();                    //Determine if motors should be operating in pulse-drive mode
  getPulseWidth();                      //Determine pulse width for pulse-drive mode

  if(motor0_CheckUpdate())              //Determine if motor0 should be updated
  {
    if(motor0_UsePulse)
    {
      motor0_UpdatePulse();             //Update motor0 with pulse-stepping
    }
    else
    {
      motor0_Update();                  //Update motor0 without pulse-stepping
    }
  }
  else
  {
    delayMicroseconds(compensatedDelay1); //Introduce delay to maintain loop execution time.
  }

  if(motor1_CheckUpdate())              //Determine if motor1 should be updated
  {
    if(motor1_UsePulse)
    {
      motor1_UpdatePulse();             //Update motor1 with pulse-stepping
    }
    else
    {
      motor1_Update();                  //Update motor1 without pulse-stepping
    }
  }
  else
  {
    delayMicroseconds(compensatedDelay1); //Introduce delay to maintain loop execution time.
  }

  if(motor2_CheckUpdate())              //Determine if motor2 should be updated
  {
    if(motor2_UsePulse)
    {
      motor2_UpdatePulse();             //Update motor2 with pulse-stepping
    }
    else
    {
      motor2_Update();                  //Update motor2 without pulse-stepping
    }
  }
  else
  {
    delayMicroseconds(compensatedDelay1); //Introduce delay to maintain loop execution time.
  }

  if(motor3_CheckUpdate())              //Determine if motor3 should be updated
  {
    if(motor3_UsePulse)
    {
      motor3_UpdatePulse();             //Update motor3 with pulse-stepping
    }
    else
    {
      motor3_Update();                  //Update motor3 without pulse-stepping
    }
  }
  else
  {
    delayMicroseconds(compensatedDelay1); //Introduce delay to maintain loop execution time.
  }
  
  delayMicroseconds(compensatedDelay0);  //Delay the loop for a specified time to set update rate
  

  
}
