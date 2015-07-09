#!/usr/bin/python
import argparse 
import time 
import serial 
import sys

def serDelWrite(ser,line,delay):
      for c in line:
            ser.write(c)
            time.sleep(delay)

def drawProgressBar(percent, barLen):
    sys.stdout.write("\r")
    progress = ""
    for i in range(barLen):
        if i < int(barLen * percent):
            progress += "="
        else:
            progress += " "
    sys.stdout.write("[ %s ] %.2f%%" % (progress, percent * 100))
    sys.stdout.flush()

parser = argparse.ArgumentParser(description='Hex Programmer Utility')
parser.add_argument('-s','--serial', help='Serial Port Device File', required=True)
parser.add_argument('-m','--mode',help='Set Mode, F or C',default='f')

args         = vars(parser.parse_args())
port         = args['serial']
mode         = args['mode']

ser = serial.Serial(
	port=port,
	baudrate=115200,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE, 
	bytesize=serial.EIGHTBITS,
	timeout=.1
)


temp5string = "t5\r"
temp4string = "t4\r"
temp3string = "t3\r"
temp0string = "t0\r"
ret = [0,0,0,0,0,0,0,0,0,0,0]
temp = [0,0,0,0,0,0]
while(1):
	for i in range (6):
		if (i != 2) and (i != 1):
			ret = [0,0,0,0,0,0,0,0,0,0,0]
			ser.write("t"+str(i)+"\r") 
			ret[0] = ser.read()
			ret[1] = ser.read()
			ret[2] = ser.read()
			ret[3] = ser.read()
			ret[4] = ser.read()
			ret[5] = ser.read()
			ret[6] = ser.read()
			ret[7] = ser.read()
			ret[8] = ser.read()
			ret[9] = ser.read()
			ret[10] = ser.read()
			temp[i] = ret[4] + ret[5] + ret[6]
#			print ret
#			time.sleep(1)

	sys.stdout.write("\r")
	if mode == 'f':
		sys.stdout.write("Temperature Sensors 5: " + str(int(temp[5])*9/5 + 32) + " || 4: " + str(int(temp[4])*9/5 + 32) + " || 3: " + str(int(temp[3])*9/5 + 32) + " || 0: " + str(int(temp[0])*9/5 + 32))
	
	else:
		sys.stdout.write("Temperature Sensors 5: " + str(int(temp[5])) + " || 4: " + str(int(temp[4])) + " || 3: " + str(int(temp[3])) + " || 0: " + str(int(temp[0])))
		
	sys.stdout.flush()
	sys.stdout.flush()
	sys.stdout.flush()
	sys.stdout.flush()
	time.sleep(.2)	

      
      
      
