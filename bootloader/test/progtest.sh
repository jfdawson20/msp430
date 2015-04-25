#!/bin/bash
        for i in `seq 1 10`; 
	do
            python Programmer/hexprogrammer.py -s /dev/ttyUSB0 -x bin/boot.hex -m p
        done
