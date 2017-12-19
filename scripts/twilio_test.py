"""Send a text message using twilio on click of button"""
import os
import serial
import time
from twilio.rest import TwilioRestClient


# Serial COMM
# SERIAL_PORT = '/dev/cu.usbserial-A5050PKK' # USB FTDI
SERIAL_PORT = '/dev/cu.BoltedButton-DevB' # Bluetooth
BAUD_RATE = 57600
sp = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout = 5)
sp.flush()


# Twilio Config
account_sid = os.environ.get('TWILIO_SID')
auth_token  = os.environ.get('TWILIO_AUTH')
client = TwilioRestClient(account_sid, auth_token)


need_service = False


while (1):
    response = sp.read(1)
    if response:
        if response == 'S':
            need_service = not need_service

            # If needs service flash LED
            if need_service:
                sp.write('F5\r'.encode('utf-8'))

                try:
                    # Send text
                    message = client.messages.create(body="Need service!",
                        to=os.environ.get('TWILIO_TO', '+15555555555'),
                        from_=os.environ.get('TWILIO_FROM', '+15555555556'))

                    print(message.sid)
                except twilio.TwilioRestException as e:
                    print e
            else:
                sp.write('N')
