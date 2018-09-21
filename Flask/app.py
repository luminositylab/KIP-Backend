#####################################################################
#                                                                   #
#   `app.py` is the main server for controlling KIP using REST API  #
#   This software is for educational purposes.                      #
#                                                                   #
#   License: https://www.mozilla.org/en-US/MPL/                     #
#   Written by GowanR <g@gmichaelrowland.com>                       #
#                                                                   #
#####################################################################


# 50 ms rate limit (20-100ms)

from flask import Flask
from flask import Flask, flash, jsonify, redirect, Response, render_template, request, session
import os
import json
import time
from access_control import crossdomain
from BufferedStepperPacket import BufferedStepperPacket
from ServoStates import ServoStates
from time import sleep
import threading

from Controls import tank_drive

app = Flask(__name__)

SUCCESS = "success"
BAD_QUERYSTRING = "bad querystring"
# json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'})

servo_state = ServoStates()

@app.route('/')
def index():
    return SUCCESS


@app.route('/set/servo/<int:servo_id>/<int:servo_position>')
@crossdomain(origin="*")
def set_servo_position(servo_id, servo_position):
    servo_state.servo_state[servo_id] = servo_position
    servo_state.update_servos()
    return SUCCESS

@app.route('/get/sensor/<int:sensor_id>')
@crossdomain(origin="*")
def get_sensor_data(sensor_id):
    # get and send sensor data
    return SUCCESS


@app.route('/drive/<left>/<right>')
@crossdomain(origin="*")
def set_drive(left, right):
    try:
        l = float(left)
        r = float(right)
        tank_drive(l, r)
    except Exception as inst:
        return str(inst)
    return SUCCESS



@app.route('/set/motor/speed/<int:motor_id>/<int:direction>/<int:speed>')
@crossdomain(origin="*")
def set_motor_speed(motor_id, direction, speed):
    BufferedStepperPacket(motor_id, 1, direction, 1, abs(speed), 0, 0, 0, 0)
    return SUCCESS

def kip_main():
  threading.Timer(5.0, kip_main).start()
  print "Hello, World!"

if __name__ == '__main__':
    kip_main()
    app.run(debug=True, host='0.0.0.0')
