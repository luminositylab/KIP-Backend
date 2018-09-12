#MYRA/KIP sensor data request function
#(C) 2018 The Luminosity Lab

#RequestSensorData() Function:
#This function writes a packet to the serial UART pins in order to request sensor data from the controller.
#The function should be interpreted using Python 3.

#This function has no input arguments, and returns a tuple containing float values for 6 sensors.

def RequestSensorData():
    #Specify constants:
    packetResetPin = 7
    resetDelay = 0.01
    serialWaitDelay = 0.1
    
    #Construct first byte in the packet------------------------------------------------------------
    
    #Set bit 0 high to specify reception by the servo/sensor controller:
    packetByte0 = 1

    #Set bit 1 low to specify a sensor data request:
    packetByte0 = (packetByte0 | (0 << 1))

    #Construct second byte in the packet-----------------------------------------------------------

    packetByte1 = 0

    #Construct third byte in the packet-----------------------------------------------------------

    packetByte2 = 0

    #Construct fourth byte in the packet-----------------------------------------------------------

    packetByte3 = 0

    #Construct fifth byte in the packet-----------------------------------------------------------

    packetByte4 = 0

    #Construct sixth byte in the packet-----------------------------------------------------------

    packetByte5 = 0

    #Configure output and transmit packet to serial port------------------------------------------

    #Set up GPIO for packet reset pin:
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(packetResetPin, GPIO.OUT)
    GPIO.output(packetResetPin, GPIO.LOW)
    
    #Initialize a serial object which will be used to interact with the Rx/Tx port.
    #These parameters are set to the same configuration required by the Arduino serial interface.
    serialObject = serial.Serial(
        port = '/dev/ttyS0',
        baudrate = 250000,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS,
        timeout = 1
    )

    #Reset microcontroller packet receiver using packet reset pin:
    GPIO.output(packetResetPin, GPIO.HIGH)
    time.sleep(resetDelay)
    GPIO.output(packetResetPin, GPIO.LOW)

    #Transmit packet:
    serialObject.write(bytes([packetByte0, packetByte1, packetByte2, packetByte3, packetByte4, packetByte5]))

    #Get serial input data from the microcontroller:
    time.sleep(serialWaitDelay)
    sensorData_0 = float(serialObject.readline())
    sensorData_1 = float(serialObject.readline())
    sensorData_2 = float(serialObject.readline())
    sensorData_3 = float(serialObject.readline())
    sensorData_4 = float(serialObject.readline())
    sensorData_5 = float(serialObject.readline())

    #Return the data as a tuple:
    return sensorData_0, sensorData_1, sensorData_2, sensorData_3, sensorData_4, sensorData_5




