import time
import serial
import RPi.GPIO as GPIO

#BufferedStepperPacket() Function:
#This function writes a packet to the serial UART pins in order to dictate the behavior of a stepper motor.
#The function should be interpreted using Python 3.
#(C) 2018 The Luminosity Lab

#Input arguments:
#MotorNum: The number (0-3) of the motor to be controlled.
#UsePulseDrive: Boolean value to determine whether or not to employ pulse-driving at lower speeds.
#Direction: Boolean value indicating forward/reverse rotation direction.
#ForcePacket: Boolean value indicating whether to force this packet to the front of the buffer.
                #(Note that this deletes other data in the buffer)
#Steps0: Integer value (0-100) indicating the approximate number of steps to be executed in an interval.
#Steps1: Integer value (0-100) indicating the approximate number of steps to be executed in an interval.
#Steps2: Integer value (0-100) indicating the approximate number of steps to be executed in an interval.
#Steps3: Integer value (0-100) indicating the approximate number of steps to be executed in an interval.
#Steps4: Integer value (0-100) indicating the approximate number of steps to be executed in an interval.

#Please note that motor stability is not guaranteed above 200 RPM.
#Buffer size is approximately 250 intervals (default interval is 100ms)
#Note: For extended sequences of operations, data should be sent in slight advance to reduce dead stops.

def BufferedStepperPacket(MotorNum, UsePulseDrive, Direction, ForcePacket, Steps0, Steps1, Steps2, Steps3, Steps4):

    #Sepcify constants:
    PacketResetPin = 7
    ResetDelay = 0.01

    #Construct first byte in the packet------------------------------------------------------------

    #Set bit 0 low to specify reception by the stepper controller:
    PacketByte0 = 0

    #Set bit 1 to UsePulseDrive:
    PacketByte0 = (PacketByte0 | (bool(UsePulseDrive) << 1))

    #Set bit 2 to Direction:
    PacketByte0 = (PacketByte0 | (bool(Direction) << 2))

    #Set bit 3 depending on the motor number:
    if ((MotorNum == 1) or (MotorNum == 3)):
        PacketByte0 = (PacketByte0 | (1 << 3))

    #Set bit 4 depending on the motor number:
    if ((MotorNum == 2) or (MotorNum == 3)):
        PacketByte0 = (PacketByte0 | (1 << 4))

    #Set bit 5 to ForcePacket:
    PacketByte0 = (PacketByte0 | (bool(ForcePacket) << 5))

    #Construct second byte in the packet----------------------------------------------------------

    PacketByte1 = int(Steps0)

    #Verify that PacketByte1 is between 0 and 100:
    if PacketByte1 > 100:
        PacketByte1 = 100
    if PacketByte1 < 0:
        PacketByte1 = 0

    #Construct third byte in the packet-----------------------------------------------------------

    PacketByte2 = int(Steps1)

    #Verify that PacketByte2 is between 0 and 100:
    if PacketByte2 > 100:
        PacketByte2 = 100
    if PacketByte2 < 0:
        PacketByte2 = 0

    #Construct fourth byte in the packet----------------------------------------------------------

    PacketByte3 = int(Steps2)

    #Verify that PacketByte3 is between 0 and 100:
    if PacketByte3 > 100:
        PacketByte3 = 100
    if PacketByte3 < 0:
        PacketByte3 = 0

    #Construct fifth byte in the packet-----------------------------------------------------------

    PacketByte4 = int(Steps3)

    #Verify that PacketByte4 is between 0 and 100:
    if PacketByte4 > 100:
        PacketByte4 = 100
    if PacketByte4 < 0:
        PacketByte4 = 0

    #Construct sixth byte in the packet-----------------------------------------------------------

    PacketByte5 = int(Steps4)

    #Verify that PacketByte5 is between 0 and 100:
    if PacketByte5 > 100:
        PacketByte5 = 100
    if PacketByte5 < 0:
        PacketByte5 = 0

    #Configure output and transmit packet to serial port------------------------------------------

    #Set up GPIO for packet reset pin:
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PacketResetPin, GPIO.OUT)
    GPIO.output(PacketResetPin, GPIO.LOW)

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
    GPIO.output(PacketResetPin, GPIO.HIGH)
    time.sleep(ResetDelay)
    GPIO.output(PacketResetPin, GPIO.LOW)

    #Transmit packet:
    serialObject.write(bytes([PacketByte0, PacketByte1, PacketByte2, PacketByte3, PacketByte4, PacketByte5]))
