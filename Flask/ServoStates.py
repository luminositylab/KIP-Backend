from enum import Enum
from SetServoPositions import SetServoPositions
import RPi.GPIO as GPIO
import serial

class ServoID(Enum):
    lWrist = 0
    rWrist = 1
    lGripper = 2
    rGripper = 3

class ServoStates:
    def __init__(self):
        self.servo_state = [0,0,0,0]

    def update_servos(self):
        SetServoPositions(
        self.servo_state[0],
        self.servo_state[1],
        self.servo_state[2],
        self.servo_state[3]
        )

    def left_wrist_down(self):
        self.servo_state[ServoID.lWrist.value] = 10
        self.update_servos()

    def left_wrist_up(self):
        self.servo_state[ServoID.lWrist.value] = 170
        self.update_servos()

    def right_wrist_down(self):
        self.servo_state[ServoID.rWrist.value] = 10
        self.update_servos()

    def right_wrist_up(self):
        self.servo_state[ServoID.rWrist.value] = 170
        self.update_servos()

    def left_gripper_open(self):
        self.servo_state[ServoID.lGripper.value] = 170
        self.update_servos()


    def left_gripper_close(self):
        self.servo_state[ServoID.lGripper.value] = 10
        self.update_servos()


    def right_gripper_open(self):
        self.servo_state[ServoID.rGripper.value] = 170
        self.update_servos()


    def right_gripper_close(self):
        self.servo_state[ServoID.rGripper.value] = 10
        self.update_servos()
