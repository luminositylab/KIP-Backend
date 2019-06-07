# ATMegaCOM.py - a script for sending simple teleop drive commands to the ATMega on the KIP baard.
# Sends over i2c on raspberry pi, make sure i2c is enabled and baud is set to 400kHz in /ect/config.txt
# Version 0.1.0
# Written by GowanR (g@gmichaelrowland.com) 2019 
# The Luminosity Lab (c) 2019

from smbus import SMBus
import time
import math
SYNC_BYTE = 255
# Packet structure:
# Packet consists of four bytes.
# Byte 0: sync byte
# Byte 1: left motor speed
# Byte 2: right motor speed
# Byte 3: arm position

class ATMegaCOM:
    def __init__(self):
        self.i2c_addr = 0x8
        self.i2c_bus = SMBus(1)

        self.total_sends = 0
        self.failed_sends = 0
    # sends the byte c over the i2c bus, will catch errors and keep trying to send the byte
    def send_over_bus_persist(self, c):
        self.total_sends += 1
        try:
            self.i2c_bus.write_byte(self.i2c_addr, c)
        except IOError:
            self.failed_sends += 1
            self.send_over_bus_persist(c)
    # alias for persistant sending
    def send(self, c):
        self.send_over_bus_persist(c)
    def map(v, in_min, in_max, out_min, out_max):
        # Check that the value is at least in_min
        if v < in_min:
            v = in_min
        # Check that the value is at most in_max
        if v > in_max:
            v = in_max
        return (v - in_min) * (out_max - out_min) // (in_max - in_min) + out_min
    def joystickToDiff(self, x, y, minJoystick, maxJoystick, minSpeed, maxSpeed):	# If x and y are 0, then there is not much to calculate...
        if x == 0 and y == 0:
            return (0, 0)
    

	# First Compute the angle in deg
	# First hypotenuse
        z = math.sqrt(x * x + y * y)

	# angle in radians
        rad = math.acos(math.fabs(x) / z)

	# and in degrees
        angle = rad * 180 / math.pi

	# Now angle indicates the measure of turn
	# Along a straight line, with an angle o, the turn co-efficient is same
	# this applies for angles between 0-90, with angle 0 the coeff is -1
	# with angle 45, the co-efficient is 0 and with angle 90, it is 1

        tcoeff = -1 + (angle / 90) * 2
        turn = tcoeff * math.fabs(math.fabs(y) - math.fabs(x))
        turn = round(turn * 100, 0) / 100

	# And max of y or x is the movement
        mov = max(math.fabs(y), math.fabs(x))

	# First and third quadrant
        
        if (x >= 0 and y >= 0) or (x < 0 and y < 0):
            rawLeft = mov
            rawRight = turn
        else:
            rawRight = mov
            rawLeft = turn

	# Reverse polarity
        if y < 0:
            rawLeft = 0 - rawLeft
            rawRight = 0 - rawRight

	# minJoystick, maxJoystick, minSpeed, maxSpeed
	# Map the values onto the defined rang
        rightOut = int(rawRight)#map(rawRight, minJoystick, maxJoystick, minSpeed, maxSpeed)
        leftOut = int(rawLeft) #map(rawLeft, minJoystick, maxJoystick, minSpeed, maxSpeed)

        return (rightOut, leftOut)
    
    # if fed from the react app, x_joy and y_joy will have range [-1,1]
    def update_robot(self, x_joy, y_joy, arm_pos):
        global SYNC_BYTE
        x_joy *= 127
        y_joy *= 127
        arm_pos *= 127
        x_joy = int(x_joy)
        y_joy = int(-y_joy)
        arm_pos = int(arm_pos)
        print("arm pos: ", arm_pos)
        (lm, rm) = self.joystickToDiff(x_joy, y_joy, -127, 127, -127, 127)
        #print("x value: ", lm, "| y value: ", rm)
        self.send(SYNC_BYTE)
        self.send(lm)
        self.send(rm)
        self.send(arm_pos)


