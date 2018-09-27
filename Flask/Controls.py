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

def tank_drive(left, right):
    d_left = int(left > 0)
    d_right = int(right > 0)
    BufferedStepperPacket(0, 1, d_left, 1, abs(left), abs(left), abs(left), abs(left), abs(left))
    BufferedStepperPacket(1, 1, d_right, 1, abs(right), abs(right), abs(right), abs(right), abs(right))

class KIP_State:
    def __init__(self):
        self.left = 0
        self.right = 0
        print("Init state")
    def set_drive(self, left, right):
        self.left = left
        self.right = right
    def get_drive(self):
        return self.left, self.right

class StateManager(threading.Thread):
    def __init__(self, queue, kwargs=None):
        threading.Thread.__init__(self, args=(), kwargs=None)
        self.queue = queue
        self.daemon = True

    def run(self):
        while(True):
            (left, right) = self.queue.get()
            if left is None or right is None:
                return
            self.drive(left, right)
            sleep(1/20.0)

    def drive(self, left, right):
        d_left = int(left > 0)
        d_right = int(right > 0)
        BufferedStepperPacket(0, 1, d_left, 1, abs(left), abs(left), abs(left), abs(left), abs(left))
        BufferedStepperPacket(1, 1, d_right, 1, abs(right), abs(right), abs(right), abs(right), abs(right))
