"""Listens on a serial connection for byte combinations to control service"""
import serial


SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
# SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600
HOST = '192.168.0.171'
PORT = 5000

sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()

while (1):
    response = sp.read(1)
    if response:
        print response
