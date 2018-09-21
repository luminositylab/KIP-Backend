from BufferedStepperPacket import BufferedStepperPacket
from ServoStates import ServoStates


def turn(speed):
    direction = int(speed > 0)
    BufferedStepperPacket(0, 1, direction, 1, abs(speed), 0, 0, 0, 0)
    BufferedStepperPacket(1, 1, direction, 1, abs(speed), 0, 0, 0, 0)

def tank_drive(left, right):
    d_left = int(left > 0)
    d_right = int(right > 0)
    BufferedStepperPacket(0, 1, d_left, 1, abs(speed), 0, 0, 0, 0)
    BufferedStepperPacket(1, 1, d_right, 1, abs(speed), 0, 0, 0, 0)
