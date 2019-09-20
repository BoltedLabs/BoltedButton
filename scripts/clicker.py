"""Listens on a serial connection for byte combinations to control service"""
import serial
import keyboard


# SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600

sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()

while (1):
    response = sp.read(1)
    if response:
        print(response)

        if response == b'S':
            keyboard.press_and_release('right')
        if response == b'D':
            keyboard.press_and_release('left')
        if response == b'T':
            pass
