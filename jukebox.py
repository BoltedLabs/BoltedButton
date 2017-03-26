"""Listens on a serial connection for byte combinations to control music"""
import serial
import subprocess
import time


# SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600

sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()

while (1):
    response = sp.read(1)
    if response == 'O':
        print 'Ready|'
    elif response == 'N':
        print 'Skipping song...'

        script = """/usr/bin/osascript<<END
        tell application "iTunes"
            next track
        end tell
        END"""
        subprocess.call(script, shell=True)

        # Stall for next skip.
        time.sleep(0.3)
