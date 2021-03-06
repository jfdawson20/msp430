#!/usr/bin/python
import argparse 
import time 
import serial 
import sys

#serial port write with delay function
def serDelWrite(ser,line,delay):
      for c in line:
            ser.write(c)
            time.sleep(delay)

#progress bar update and draw function
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

#start of main program 
#argument parsing 
parser = argparse.ArgumentParser(description='Hex Programmer Utility')
parser.add_argument('-s','--serial', help='Serial Port Device File', required=True)
parser.add_argument('-m','--mode',help='Mode: p = programming, r = read address range', default='p')
parser.add_argument('-x','--hex', help='Hex Input File')
parser.add_argument('-t','--timeout', help='Number of write retries allowed', default='200')
parser.add_argument('-b','--base',help='Base address for read',default='0xFFE0')
parser.add_argument('-e','--end',help='End address for read',default='0xFFFF')

args         = vars(parser.parse_args())
port         = args['serial']
mode         = args['mode']       
hexFile      = args['hex']
timeoutLimit = int(args['timeout'],10)
baseAddr     = int(args['base'],16)
endAddr      = int(args['end'],16)

#setup serial port 
ser = serial.Serial(
	port=port,
	baudrate=115200,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE, 
	bytesize=serial.EIGHTBITS,
	timeout=.1
)
print mode
if (mode == 'p'):
	#open hex file 
	with open(hexFile) as f:
		fileContent = f.readlines()

    

print "--------------------------------"
print "MSP430 Serial Bootloader"
print "Author : J.Dawson"
print "Date   : 1/15/2015"
print "--------------------------------"


if (mode == 'p'): #programming mode selected 
      print ""
      print ""
      print "Port   : " + port 
      print "Source : " + hexFile  
      print "Starting Serial Boot Programming"

      print "Connecting to board\n"
      ser.write('b\r')
      ret = ser.read()
      init_timeout = 0
      while(ret != '+' and init_timeout < 10):
            ser.write('b\r')
	    ret = ser.read()
            time.sleep(.1)
            init_timeout = init_timeout +1

      if(init_timeout == 10 and ret != '+'):
            print "Failed to lock to msp430, reset dut and retry connection\n"
      else:	
            n = len(fileContent)
            i = 0
            timeout = 0
            stop = 0 
            for lines in fileContent:    
	           if (stop == 1): 
		          break; 
	           drawProgressBar(float(i)/float(n-1), barLen = 30)	
	           ser.write(lines) 
	           ret = ser.read()
	           # bootloader responds with '-' ascii character for failed transmissions
                   # responds with '+' for success 
                   # while failing retry until timeout expires 	
	           while(ret == '-' or len(ret) == 0):
		           ser.write(lines)
		           ret = ser.read()
		           timeout = timeout + 1
		           if (timeout > timeoutLimit):
	                         stop = 1;
			         print "Programming Failed at line ("+str(i)+") : " + lines
			         break;
                   i = i + 1


      print ""
      print ""
      print "Finished\n"
      
      
elif (mode == 'r'): #read range of addresses
      print "read mode\n"

      ser.write('R'+hex(baseAddr)[2:].upper()+hex(endAddr)[2:].upper()+'\r')
      ret = ser.read()
      while(ret == '-'):
	ser.write('R'+hex(baseAddr)[2:].upper()+hex(endAddr)[2:].upper()+'\r')
	ret = ser.read()

      print "Address: " + hex(baseAddr) + " <> Data: " + ret.encode("hex")
      for i in range((baseAddr+1),endAddr):
            ret = ser.read()
            print "Address: " + hex(i) + " <> Data: " + ret.encode("hex")
            
else:
      print "Invalid Mode Selected\n"
      print "Exiting\n"
      
      
      
