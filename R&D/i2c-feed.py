from smbus import SMBus
import time
global addr
addr = 0x8 # bus address
global bus
bus = SMBus(1) # indicates /dev/ic2-1
#bus.write_byte(addr, 0x1) # switch it on
#input("Press return to exit")
#bus.write_byte(addr, 0x0) # switch it on
global total_sends
global failed_sends
total_sends = 0
failed_sends = 0
def send_over_bus_persist(c):
    global total_sends
    global failed_sends
    global addr
    global bus

    total_sends += 1
    try:
        bus.write_byte(addr, ord(c))
    except IOError:
        failed_sends += 1
        send_over_bus_persist(c)


while True:
    word = str(input("Input commmand instructions:"))
    send_over_bus_persist(chr(255))
    for c in word:
        #time.sleep(0.05)
        send_over_bus_persist(c)

    #print("Send Failure: " + str(failed_sends/total_sends) + "%")
    print("Total Sends: ", total_sends, " Failed Sends: ", failed_sends)
