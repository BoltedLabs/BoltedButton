"""Listens on a serial connection for byte combinations to control music"""
import serial
import subprocess
import time
import sys


# SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600

sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()

script = ''

if __name__ == '__main__':
    if len(sys.argv) > 1:
        music_service = sys.argv[1]
    else:
        music_service = 'itunes'

    while (1):
        response = sp.read(1)
        if response == 'R':
            print 'Ready|'
        elif response == 'S':
            print 'Skipping song...'

            if music_service == 'spotify':
                script = """/usr/bin/osascript<<END
                tell application "Spotify" to next track
                END"""
            else:
                script = """/usr/bin/osascript<<END
                tell application "iTunes"
                    next track
                end tell
                END"""
        elif response == 'D':
            script = """/usr/bin/osascript<<END
            tell application "iTunes"
            	playpause
            end tell
            END"""
            sp.write('F3\r'.encode('utf-8'))

        if script:
            subprocess.call(script, shell=True)

            # Stall for next skip.
            time.sleep(0.5)
