#####################################################################
#                                                                   #
#   `app.py` is the main server for controlling KIP using REST API  #
#   This software is for educational purposes.                      #
#                                                                   #
#   License: Luminosity Lab (c) 2018                                #
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
from queue import Queue
from Controls import StateManager


app = Flask(__name__)

SUCCESS = "success"
BAD_QUERYSTRING = "bad querystring"

# json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'})

servo_state = ServoStates()
q = Queue()
manager = StateManager(q)

user_inputs = {
    'left_drive':0.0,
    'right_drive':0.0,
    'left_arm':0.0,
    'right_arm':0.0,
    'left_wrist':0.0,
    'right_wrist':0.0,
    'left_claw':0.0,
    'right_claw':0.0
}
def update_manager():
    manager.queue.put(user_inputs)

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
        global user_inputs
        user_inputs['left_drive'] = float(left)
        user_inputs['right_drive'] = float(right)
        update_manager()
        

    except Exception as inst:
        return str(inst)
    return SUCCESS


if __name__ == '__main__':
    manager.start()
    app.run(debug=True, host='0.0.0.0')
