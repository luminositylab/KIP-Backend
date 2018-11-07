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
        left_drive_motor = Motor(0)
        right_drive_motor = Motor(1)
        left_drive_motor.reverse()

        left_shoulder_motor = Motor(2)
        right_shoulder_motor = Motor(3)
        
        left_shoulder_motor.set_reduction(10)
        right_shoulder_motor.set_reduction(10)
        # end user set up code

        while(True):
            user_inputs = self.queue.get()
            if user_inputs is None:
                continue
            if user_inputs['left_drive'] == 0 or user_inputs['right_drive'] == 0:
                pass
            # begin user defined code
            
            #left_drive_motor.set(user_inputs['left_drive'])
            #right_drive_motor.set(user_inputs['right_drive'])
            
            left_shoulder_motor.change_angle(-90) 
            #for i in range(10):
            #    left_drive_motor.set(25)
            #    right_drive_motor.set(25)
            #    sleep(1/30.0)
            
            #for i in range(10):
            #    left_drive_motor.set(25)
            #    right_drive_motor.set(-25)
            
            
            #print('Right: ', user_inputs['right_drive'], ' Left: ', user_inputs['left_drive'])
            # left_shoulder_motor.set(user_inputs['left_arm'])
            # right_shoulder_motor.set(user_inputs['right_arm'])



            # end use defined code
        
            sleep(1.0/20.0)

def degrees_to_steps(theta, reduction): # assume 1.8 degree step
    return theta * (reduction/1.8)
        

def chunks(l, n):
    for i in range(0,len(l), n):
        yield l[i:i+n]
def packetize_steps(d_steps, s):
    step_counter = d_steps
    step_set = []
    while abs(step_counter) > 0:
        if step_counter >= s:
            step_set.append(s)
            step_counter -= s
        elif step_counter <= -s:
            step_set.append(-s)
            step_counter += s
        else:
            step_set.append(step_counter)
            step_counter = 0
    return step_set

class Motor:
    def __init__(self, motor_id):
        self.id = motor_id
        self.angle = 0 # theoretical angle of stepper motor, doesn't account for lost steps
        self.steps = 0 
        self.max_steps_per_packet = 50 # Maximun steps per 100ms this determines max RPM and pushing torque
        self.reversed = False
        self.reduction = 1 # gearbox reduction, default 1:1
    
    def set_reduction(self, reduction):
        self.reduction = reduction
    
    def set_absolute_angle(self, angle):
        d_theta = self.angle - angle
        self.angle = angle
        d_steps = degrees_to_steps(d_theta, self.reduction)
        step_set = packetize_steps(d_steps, self.max_steps_per_packet)
        self.send_packet(step_set)
    
    def change_angle(self, angle):
        d_steps = degrees_to_steps(angle, self.reduction)
        step_set = packetize_steps(d_steps, self.max_steps_per_packet)
        self.send_packet(step_set)
    def reverse(self):
        self.reversed = not self.reversed
    
    def set(self, speed):
        try:
            d_speed = int(speed > 0)
            if self.reversed:
                d_speed = int(not bool(d_speed))
            #print("Running Motor # ", self.id, " at speed ", abs(speed), " and direction ", d_speed)
            BufferedStepperPacket(self.id, 1, d_speed, 1, abs(speed), abs(speed), abs(speed), abs(speed), abs(speed))
        except Exception as inst:
            print(inst)
            return False
        return True

    def send_packet(self, packet):
        if len(packet) < 5:
            # construct packet if it's too small
            for i in range(5 - len(packet)):
                packet.append(0)
        try:
            d_speed = int(packet[0] > 0)
            if self.reversed:
                d_speed = int(not bool(d_speed))
            master_packet = list(chunks(packet, 5))
            for p in master_packet:
                if len(p) < 5:
                    for i in range(5-len(p)):
                        p.append(0)
                BufferedStepperPacket(self.id, 1, d_speed, 1, abs(p[0]) or 0, abs(p[1]) or 0, abs(p[2]) or 0, abs(p[3]) or 0, abs(p[4]) or 0)
        except Exception as inst:
            print(inst)
            return False
        return True
