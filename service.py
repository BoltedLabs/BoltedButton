"""Listens on a serial connection for byte combinations to control service"""
import serial
from socketIO_client import SocketIO, LoggingNamespace
import time


# SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600
HOST = '192.168.0.171'
PORT = 5000

sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()

while (1):
    response = sp.read(1)
    if response == 'O':
        print 'Ready|'
    elif response == 'N':
        print 'Need service!'

        # Update screen
        with SocketIO(HOST, PORT, LoggingNamespace) as socketIO:
            socketIO.emit('button_single_click', {"id":1, "state":True})
            socketIO.wait(seconds=1)
    elif response == 'S':
        print 'Button off'

        # Update screen
        with SocketIO(HOST, PORT, LoggingNamespace) as socketIO:
            socketIO.emit('button_single_click', {"id":1, "state":False})
            socketIO.wait(seconds=1)

        # Stall for next skip
        time.sleep(0.3)
