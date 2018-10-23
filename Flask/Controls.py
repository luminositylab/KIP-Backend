#####################################################################
#                                                                   #
#   `Controls.py` defines all of the state control in second thread #
#   This software is for educational purposes.                      #
#                                                                   #
#   License: Luminosity Lab (c) 2018                                #
#   Written by GowanR <g@gmichaelrowland.com>                       #
#                                                                   #
#####################################################################

from BufferedStepperPacket import BufferedStepperPacket
from ServoStates import ServoStates
import threading
import time
from time import sleep
from queue import Queue

def turn(speed):
    direction = int(speed > 0)
    BufferedStepperPacket(0, 1, direction, 1, abs(speed), 0, 0, 0, 0)
    BufferedStepperPacket(1, 1, direction, 1, abs(speed), 0, 0, 0, 0)


class StateManager(threading.Thread):
    def __init__(self, queue, kwargs=None):
        threading.Thread.__init__(self, args=(), kwargs=None)
        self.queue = queue
        self.daemon = True

    def run(self):
        # begin user set up code
        left_drive_motor = Motor(0)
        right_drive_motor = Motor(1)

        left_shoulder_motor = Motor(3)
        right_shoulder_motor = Motor(4)
        # end user set up code

        while(True):
            user_inputs = self.queue.get()
            if user_inputs is None:
                return
            
            # begin user defined code
            left_drive_motor.set(user_inputs['left_drive'])
            right_drive_motor.set(user_inputs['right_drive'])

            left_shoulder_motor.set(user_inputs['left_arm'])
            right_shoulder_motor.set(user_inputs['right_arm'])

            

            # end use defined code
        
            sleep(1/20.0)

    def drive(self, left, right):
        d_left = int(left > 0)
        d_right = int(right > 0)
        BufferedStepperPacket(0, 1, d_left, 1, abs(left), abs(left), abs(left), abs(left), abs(left))
        BufferedStepperPacket(1, 1, d_right, 1, abs(right), abs(right), abs(right), abs(right), abs(right))

class Motor:
    def __init__(self, motor_id):
        self.id = motor_id

    def set(self, speed):
        d_speed = int(speed > 0)
        BufferedStepperPacket(self.id, 1, d_speed, 1, abs(speed), abs(speed), abs(speed), abs(speed), abs(speed))