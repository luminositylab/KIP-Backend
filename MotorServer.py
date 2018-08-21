from flask import Flask
from flask import Flask, flash, jsonify, redirect, Response, render_template, request, session, abort
import os
import json
import time
import serial
import RPi.GPIO as GPIO
from access_control import crossdomain
#--------------------------------------------------------------------------------------------------

#setStepperSpeeds() Function:
#This function writes a packet to the serial UART pins in order to specify stepper motor speeds.

#Input arguments:
#RPM0: A number from -600 to 600 representing 600 RPM backwards through 600 RPM forwards for motor0.
#RPM1: A number from -600 to 600 representing 600 RPM backwards through 600 RPM forwards for motor1.
#RPM2: A number from -600 to 600 representing 600 RPM backwards through 600 RPM forwards for motor2.
#RPM3: A number from -600 to 600 representing 600 RPM backwards through 600 RPM forwards for motor3.
#usePulseDrive: A boolean value indicating whether or not to use pulse driving to save power.

#Please note that motor stability is not guaranteed above 200 RPM.
#Slowest possible speed is 2.5 RPM.
#Also note that only integer values of (600/RPM_Value) are possible due to the driver algorithm.
#Non-integer values will be truncated to integer values.

def setStepperSpeeds(RPM0, RPM1, RPM2, RPM3, usePulseDrive):
    #Sepcify constants:
    dividerConversionFactor = 600.0
    packetResetPin = 7
    resetDelay = 0.01
    
    #Construct first byte in the packet------------------------------------------------------------
    
    #Set bit 0 low to specify reception by the stepper controller:
    packetByte0 = 0

    #Set bit 1 to usePulseDrive:
    packetByte0 = (packetByte0 | (usePulseDrive << 1))
    
    if RPM0 < 0:
        #Set bit 2 high if RPM0 is negative:
        packetByte0 = (packetByte0 | (1 << 2))
    if RPM1 < 0:
        #Set bit 3 high if RPM1 is negative:
        packetByte0 = (packetByte0 | (1 << 3))
    if RPM2 < 0:
        #Set bit 4 high if RPM2 is negative:
        packetByte0 = (packetByte0 | (1 << 4))
    if RPM3 < 0:
        #Set bit 5 high if RPM3 is negative:
        packetByte0 = (packetByte0 | (1 << 5))

    #Construct second byte in the packet----------------------------------------------------------

    #Determine if motor stop value (255) should be used:
    if RPM0 == 0:
        packetByte1 = 255
    else:
        #Calculate motor loop divider based on RPM:
        packetByte1 = int((dividerConversionFactor/abs(RPM0))-1)

        #If packetByte exceeds 255, set it to 255.
        if packetByte1 > 255:
            packetByte1 = 255

    #Construct third byte in the packet-----------------------------------------------------------

    #Determine if motor stop value (255) should be used:
    if RPM1 == 0:
        packetByte2 = 255
    else:
        #Calculate motor loop divider based on RPM:
        packetByte2 = int((dividerConversionFactor/abs(RPM1))-1)

        #If packetByte exceeds 255, set it to 255.
        if packetByte2 > 255:
            packetByte2 = 255

    #Construct fourth byte in the packet----------------------------------------------------------

    #Determine if motor stop value (255) should be used:
    if RPM2 == 0:
        packetByte3 = 255
    else:
        #Calculate motor loop divider based on RPM:
        packetByte3 = int((dividerConversionFactor/abs(RPM2))-1)

        #If packetByte exceeds 255, set it to 255.
        if packetByte3 > 255:
            packetByte3 = 255

    #Construct fifth byte in the packet-----------------------------------------------------------

    #Determine if motor stop value (255) should be used:
    if RPM3 == 0:
        packetByte4 = 255
    else:
        #Calculate motor loop divider based on RPM:
        packetByte4 = int((dividerConversionFactor/abs(RPM3))-1)

        #If packetByte exceeds 255, set it to 255.
        if packetByte4 > 255:
            packetByte4 = 255

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
    
#--------------------------------------------------------------------------------------------------


app = Flask(__name__)

@app.route('/') 
def index():
    return 'Hello world'

@app.route('/updatemotors')
@crossdomain(origin="*")
def update_motors():
    motor0 = request.args.get('m0')
    motor1 = request.args.get('m1')
    motor2 = request.args.get('m2')
    motor3 = request.args.get('m3')
    if not motor0 or not motor1 or not motor2 or not motor3:
        return json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'}
    
    setStepperSpeeds(float(motor0), float(motor1), float(motor2), float(motor3), 1)
    return json.dumps({'message':'success'}), 200, {'Content-Type':'application/json; charset=utf-8'}

if __name__ == '__main__':     
    app.run(debug=True, host='0.0.0.0')
