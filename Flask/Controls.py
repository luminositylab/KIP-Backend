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

class StateManager(threading.Thread):
    def __init__(self, queue, kwargs=None):
        threading.Thread.__init__(self, args=(), kwargs=None)
        self.queue = queue
        self.daemon = True

    def run(self):
        # begin user set up code
        left_drive_motor = Motor(1)
        #right_drive_motor = Motor(2)

        #left_shoulder_motor = Motor(3)
        #right_shoulder_motor = Motor(4)
        # end user set up code

        while(True):
            user_inputs = self.queue.get()
            if user_inputs is None:
                return
            #if user_inputs['left_drive'] == 0 or user_inputs['right_drive'] == 0:
            #    return
            # begin user defined code
            left_drive_motor.set(user_inputs['left_drive'])
            



            #right_drive_motor.set(user_inputs['right_drive'])
            #print('Right: ', user_inputs['right_drive'], ' Left: ', user_inputs['left_drive'])
            # left_shoulder_motor.set(user_inputs['left_arm'])
            # right_shoulder_motor.set(user_inputs['right_arm'])



            # end use defined code
        
            sleep(1.0/20.0)

class Motor:
    def __init__(self, motor_id):
        self.id = motor_id

    def set(self, speed):
        try:
            d_speed = int(speed > 0)
            print("Running Motor # ", self.id, " at speed ", abs(speed), " and direction ", d_speed)
            BufferedStepperPacket(self.id, 1, d_speed, 1, abs(speed), abs(speed), abs(speed), abs(speed), abs(speed))
        except Exception as inst:
            print(inst)
