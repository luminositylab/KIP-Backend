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

from Controls import tank_drive, KIP_State


app = Flask(__name__)

SUCCESS = "success"
BAD_QUERYSTRING = "bad querystring"
left = 0
right = 0
# json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'})

servo_state = ServoStates()
robo_state = KIP_State()

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
        right = r
        left = l

    except Exception as inst:
        return str(inst)
    return SUCCESS


# @app.route('/arm/<side>/<speed>')
# @crossdomain(origin="*")
# def set_arm_speed(side, speed):
#     direction = int(speed) > 0
#     if(side == "left"):
#         motor_id = 2
#     else:
#         motor_id = 3
#     BufferedStepperPacket(motor_id, 1, direction, 1, abs(speed), 0, 0, 0, 0)

@app.route('/set/motor/speed/<int:motor_id>/<int:direction>/<int:speed>')
@crossdomain(origin="*")
def set_motor_speed(motor_id, direction, speed):
    BufferedStepperPacket(motor_id, 1, direction, 1, abs(speed), 0, 0, 0, 0)
    return SUCCESS

def kip_main():
  threading.Timer((1.0/20.0), kip_main).start()
  d_left = int(left > 0)
  d_right = int(right > 0)
  BufferedStepperPacket(0, 1, d_left, 1, abs(left), abs(left), abs(left), abs(left), abs(left))
  BufferedStepperPacket(1, 1, d_right, 1, abs(right), abs(right), abs(right), abs(right), abs(right))
  #tank_drive(robo_state.left, robo_state.right)

if __name__ == '__main__':
    kip_main()
    app.run(debug=True, host='0.0.0.0')
