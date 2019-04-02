#!/usr/bin/env python3

from flask_socketio import SocketIO, emit
from flask import Flask, render_template, jsonify

app = Flask(__name__)
socketio = SocketIO(app)

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

@socketio.on('connect', namespace='/kip')
def connect(sid, environ):
    print("Connect, sid ")

@socketio.on('update', namespace='/kip')
def update(sid, data):
    print("message ", data)
    sio.emit('reply', room=sid)

@socketio.on('disconnect', namespace='/kip')
def disconnect(sid):
    print('disconnect ', sid)

if __name__ == '__main__':
    print("Running Socket Server")
    socketio.run(app)
