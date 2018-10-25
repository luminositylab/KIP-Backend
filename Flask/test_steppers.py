from BufferedStepperPacket import BufferedStepperPacket
from time import sleep

while(True):
    BufferedStepperPacket(0, 1, 1, 1, 25, 25, 25, 25, 25)
    sleep(1)
