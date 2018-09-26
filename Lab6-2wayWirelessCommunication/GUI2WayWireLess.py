## This is my front end GUI
## For 2wayWirlessCommunication program

import serial
import io

myBPS = 9600
myCom = 'COM5'

mySerial = serial.Serial(myCom , myBPS)
