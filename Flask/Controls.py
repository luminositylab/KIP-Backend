from BufferedStepperPacket import BufferedStepperPacket
from ServoStates import ServoStates


def turn(speed):
    direction = int(speed > 0)
    BufferedStepperPacket(0, 1, direction, 1, abs(speed), 0, 0, 0, 0)
    BufferedStepperPacket(1, 1, direction, 1, abs(speed), 0, 0, 0, 0)

def tank_drive(left, right):
    d_left = int(left > 0)
    d_right = int(right > 0)
    BufferedStepperPacket(0, 1, d_left, 1, abs(left), 0, 0, 0, 0)
    BufferedStepperPacket(1, 1, d_right, 1, abs(right), 0, 0, 0, 0)

class KIP_State:
    def __init__(self):
        self.left = 0
        self.right = 0
    def set_drive(self, left, right):
        self.left = left
        self.right = right
