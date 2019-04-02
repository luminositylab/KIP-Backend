#!/usr/bin/env python3


# SocketServerTest.py
# Tests the local socket server.
# Written by G M Rowland // g@gmichaelrowland.com


from socketIO_client import SocketIO, LoggingNamespace

def on_connect():
    print('connect')

def on_disconnect():
    print('disconnect')

def on_update_response(*args):
    print('update response ', args)


print("Starting Socket")
socketIO = SocketIO('localhost', 500, LoggingNamespace)

socketIO.on('connect', on_connect)
socketIO.on('disconnect', on_disconnect)
#socketIO.on('update' on_update_response)

#socketIO.emit('update')
#socketIO.wait(seconds=1)
