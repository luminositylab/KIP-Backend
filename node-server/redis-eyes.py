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
discon_eye = Image.open('./i2c-eyes/discon.png').convert('1')
rightEye.begin()
leftEye.begin()

def setEyes(left, right):
    global rightEye
    global leftEye
    leftEye.clear()
    rightEye.clear()
    leftEye.image(left)
    rightEye.image(right)
    leftEye.display()
    rightEye.display()

def slowEyes():
    setEyes(cute_eye, cute_eye)
def fastEyes():
    setEyes(focused_left, focused_right)
def disconEyes():
    setEyes(discon_eye, discon_eye)

print("Starting eyes")
while(True):
    time.sleep(0.1)
    eyes = r.get("eyes")
    con = r.get("user")
    if con.decode() == "disconnected" or not con:
        disconEyes()
        continue
    if not eyes:
        continue
    if eyes.decode() == 'fast':
        fastEyes()
    if eyes.decode()== 'slow':
        slowEyes()
