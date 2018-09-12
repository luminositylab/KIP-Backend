#####################################################################
#                                                                   #
#   `app.py` is the main server for controlling KIP using REST API  #
#   This software is for educational purposes.                      #
#                                                                   #
#   License: https://www.mozilla.org/en-US/MPL/                     #
#   Written by GowanR <g@gmichaelrowland.com>                       #
#                                                                   #
#####################################################################



from flask import Flask
from flask import Flask, flash, jsonify, redirect, Response, render_template, request, session
import os
import json
import time
from access_control import crossdomain
from BufferedStepperPacket import BufferedStepperPacket
from ServoStates import ServoStates


app = Flask(__name__)

SUCCESS = "success"
BAD_QUERYSTRING = "bad querystring"
# json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'})

servo_state = ServoStates()

@app.route('/')
def index():
    return SUCCESS

@app.route('/set/servo/<int:servo_id>/<float:servo_position>')
@crossdomain(origin="*")
def set_servo_position(servo_id, servo_position):
    servo_state.servo_state[servo_id] = servo_position
    servo_state.update_servos()
    return SUCCESS

@app.route('/get/debug')
@crossdomain(origin="*")
def debug():
    servo_state.left_wrist_down()
    return SUCCESS

@app.route('/get/sensor/<int:sensor_id>')
@crossdomain(origin="*")
def get_sensor_data(sensor_id):
    # get and send sensor data
    return SUCCESS

@app.route('/set/motor/motionprofile/<int:motor_id>')
@crossdomain(origin="*")
def set_motor_motion_profile(motor_id):
    # should get querystrings and send them through serial in a rate-limited way.
    return SUCCESS

@app.route('/set/motor/<int:motor_id>/<int:position>')
@crossdomain(origin="*")
def set_motor_position(motor_id, position):

    return SUCCESS

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
