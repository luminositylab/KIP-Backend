import time
import serial
import RPi.GPIO as GPIO

#MYRA/KIP servo position selector function
#(C) 2018 The Luminosity Lab

#SetServoPositions() Function:
#This function writes a packet to the serial UART pins in order to specify 4 servo motor positions.
#The function should be interpreted using Python 3.

#Input arguments (position values):
#POS0: A number from 0-180 representing 0-180 degrees of rotation for servo0.
#POS1: A number from 0-180 representing 0-180 degrees of rotation for servo1.
#POS2: A number from 0-180 representing 0-180 degrees of rotation for servo2.
#POS3: A number from 0-180 representing 0-180 degrees of rotation for servo3.

#Please note that some servo motors may be confined to less than a full 180 degrees of motion.

def SetServoPositions(POS0, POS1, POS2, POS3):
    #Specify constants:
    packetResetPin = 7
    resetDelay = 0.01

    #Construct first byte in the packet------------------------------------------------------------

    #Set bit 0 high to specify reception by the servo/sensor controller:
    packetByte0 = 1

    #Set bit 1 high to specify use of the servo position control:
    packetByte0 = (packetByte0 | (1 << 1))

    #Construct second byte in the packet----------------------------------------------------------

    #Set packet byte to the given position value:
    packetByte1 = POS0

    #If packetByte exceeds 180, set it to 180.
    if packetByte1 > 180:
        packetByte1 = 180

    #If packetByte is less than 0, set it to 0.
    if packetByte1 < 0:
         packetByte1 = 0

    #Construct third byte in the packet----------------------------------------------------------

    #Set packet byte to the given position value:
    packetByte2 = POS1

    #If packetByte exceeds 180, set it to 180.
    if packetByte2 > 180:
        packetByte2 = 180

    #If packetByte is less than 0, set it to 0.
    if packetByte2 < 0:
        packetByte2 = 0

    #Construct fourth byte in the packet----------------------------------------------------------

    #Set packet byte to the given position value:
    packetByte3 = POS2

    #If packetByte exceeds 180, set it to 180.
    if packetByte3 > 180:
        packetByte3 = 180

    #If packetByte is less than 0, set it to 0.
    if packetByte3 < 0:
           packetByte3 = 0

    #Construct fifth byte in the packet----------------------------------------------------------

    #Set packet byte to the given position value:
    packetByte4 = POS3

    #If packetByte exceeds 180, set it to 180.
    if packetByte4 > 180:
        packetByte4 = 180

    #If packetByte is less than 0, set it to 0.
    if packetByte4 < 0:
        packetByte4 = 0

    #Construct sixth byte in the packet-----------------------------------------------------------

    packetByte5 = 0

    #Configure output and transmit packet to serial port------------------------------------------

    #Set up GPIO for packet reset pin:
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(packetResetPin, GPIO.OUT)
    GPIO.output(packetResetPin, GPIO.LOW)

    #Initialize a serial object which will be used to interact with the Rx/Tx port.
    #These parameters are set to the same configuration required by the Arduino serial interface.
    serialObject = serial.Serial(
        port = '/dev/ttyS0',
        baudrate = 250000,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS,
        timeout = 1
    )

    #Reset microcontroller packet receiver using packet reset pin:
    GPIO.output(packetResetPin, GPIO.HIGH)
    time.sleep(resetDelay)
    GPIO.output(packetResetPin, GPIO.LOW)

    #Transmit packet:
    serialObject.write(bytes([packetByte0, packetByte1, packetByte2, packetByte3, packetByte4, packetByte5]))
