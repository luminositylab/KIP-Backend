import os
import sys
import time
import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306
from PIL import Image
import redis

r = redis.Redis(host='localhost', port=6379, db=0)
RST = 24
rightEye = Adafruit_SSD1306.SSD1306_128_32(rst=RST, i2c_address=0x3c)
leftEye = Adafruit_SSD1306.SSD1306_128_32(rst=RST, i2c_address=0x3d)

focused_right = Image.open('./i2c-eyes/focus-eyes2.png').convert('1')
focused_left = Image.open('./i2c-eyes/focus-eyes.png').convert('1')
cute_eye = Image.open('./i2c-eyes/eyes-block.png').convert('1')
rightEye.begin()
leftEye.begin()

def fastEyes():
    global rightEye
    global leftEye
    global focused_right
    global focused_left
    leftEye.clear()
    rightEye.clear()
    leftEye.image(focused_left)
    rightEye.image(focused_right)
    leftEye.display()
    rightEye.display()

def slowEyes():
    global rightEye
    global leftEye
    global cute_eye
    leftEye.clear()
    rightEye.clear()
    leftEye.image(cute_eye)
    rightEye.image(cute_eye)
    leftEye.display()
    rightEye.display()

print("Starting eyes")
while(True):
    time.sleep(0.2)
    eyes = r.get("eyes")
    print("eyes: ", eyes.decode())
    if not eyes:
        print("no eyes");
        continue
    if eyes.decode() == 'fast':
        print("fast")
        fastEyes()
    if eyes.decode()== 'slow':
        print("slow")
        slowEyes()
