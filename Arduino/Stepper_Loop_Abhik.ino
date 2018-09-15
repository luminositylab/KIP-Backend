#define numMototrs 4
#define pinOffset 2

byte VMotors[numMototrs] = {128,128,128,128};//byte indicating velocity for motors (0-127) reverse (128) stationary (129-254) forward (255) hold current position

int PMotors[numMototrs] = {0,0,0,0};//the position of each motor in number of steps taken since program start

int stepsAtV[numMototrs] = {0,0,0,0};

long startMs[numMototrs] = {0,0,0,0};

int baudRate = 9600;

int holdV = 50;// out of 127

int combos = 0b100011000010001100010001100011001;

int usPulseDelay = 20;
int msUsDelay = 75;//compensated delay for 1kHz
long ms = 0;


void setup() {
  Serial.begin(baudRate);

  for (int m = 0; m < numMototrs; m++)// initialize pins
    for (int pin = 0; pin < 4; pin++)
      pinMode(pinOffset + m*4 + pin, OUTPUT);
}


void loop() {
  if (Serial.available() > 0)// take in commands
    while (Serial.available() > 0)
      if (Serial.read() == 1)
        for (int m = 0; m < numMototrs; m++) {
          VMotors[m] = Serial.read();
          startMs[m] = ms;
        }

  for (int m = 0; m < numMototrs; m++){
    if ((ms - startMs[m]) % round((float)(1000/(abs(VMotors[m]-128)*4)) * stepsAtV[m]) == 0) { //should this motor move now? // figure out the remainder stuff
      PMotors[m] += ((VMotors[m]-128) > 0) - ((VMotors[m]-128) < 0);
      stepsAtV[m]++;
    }

    for (int pin = 0; pin < 4; pin++)// update pins for that motor
      digitalWrite(pinOffset + m*4 + pin, (combos >> ((PMotors[m] % 8) * 4) + pin) & 1);
      //write(motorStartPin + pin, (selecting the desired bit from the desired 4 bit nibble of the combos indexed by the position))
  }

  delayMicroseconds(usPulseDelay);
  for (int m = 0; m < numMototrs; m++)
    if (abs(VMotors[m] - 128) > holdV)
      for (int pin = 0; pin < 4; pin++)
        digitalWrite(pinOffset + m*4 + pin, 0);

  ms++;
  delayMicroseconds(msUsDelay);
}
