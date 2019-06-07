#!/usr/bin/env python3

from flask_socketio import SocketIO, emit
from flask import Flask, render_template, jsonify
from ATMegaCOM import ATMegaCOM

app = Flask(__name__)
socketio = SocketIO(app)

class SimpleRobot:
    def __init__(self, com):
        self.left_drive = 0
        self.right_drive = 0
        self.arm_pos = 0
        self.com = com
    def drive(self, x, y):
        x = float(x)
        y = float(y)
        self.left_drive = x
        self.right_drive = y
        self.com.update_robot(self.left_drive, self.right_drive, self.arm_pos)
    def move_arm(self, v):
        v = float(v)
        self.arm_pos = (v + 1)/2
        self.com.update_robot(self.left_drive, self.right_drive, self.arm_pos)
        
com = ATMegaCOM()
kip = SimpleRobot(com)

def SUCCESS(message=""):
    if message is "":
        return {'status':'success'}
    return {'status':'success', 'message':str(message)}

def FAILURE(message=""):
    if message is "":
        return {'status':'failure'}
    return {'status':'failure', 'message':str(message)}


@app.route('/')
def index():
    return jsonify(SUCCESS())

@app.route('/arm/<value>')
def arm(value):
    #print('Got Value: ', value)
    kip.move_arm(value)
    return jsonify(SUCCESS())


@app.route('/appdrive/<x>/<y>')
def appdrive(x, y):
    #print('Got values: ', x, ', ', y)
    #print('dropped coms: ', com.failed_sends)
    #print("driving", x, ", ", y)
    kip.drive(x, y)
    return jsonify(SUCCESS())


@socketio.on('message')
def handle_message(message):
    print('received message: ' + str(message) )

@socketio.on('connect', namespace='/kip')
def connect(sid):
    print("Connect, sid ")

@socketio.on('update', namespace='/kip')
def update(sid, data):
    print("message ", data)
    # sio.emit('reply', room=sid)

@socketio.on('disconnect', namespace='/kip')
def disconnect(sid):
    print('disconnect ', sid)

if __name__ == '__main__':
    print("Running Socket Server")
    socketio.run(app, host='0.0.0.0')
