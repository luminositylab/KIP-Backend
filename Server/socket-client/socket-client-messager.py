import socketio

sio = socketio.Client()
default_ip_addr = "localhost:5000"
ip_addr = input("ip? default is" + default_ip_addr + ": ")
if not ip_addr:
    ip_addr = default_ip_addr

@sio.on('connect')
def on_connect():
    global ip_addr
    print("Connected to ", ip_addr)

@sio.on('disconnect')
def on_disconnect():
    global ip_addr
    print("Disconnected from ", ip_addr)


sio.connect('http://' + ip_addr)

data = ''
while data is not 'exit':
    data = input('packet to send: ')
    sio.emit('message', data)

sio.disconnect()
