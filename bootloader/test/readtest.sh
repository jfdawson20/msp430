#!/bin/bash
        for i in `seq 1 10`; 
        do
            python Programmer/hexprogrammer.py -s /dev/ttyUSB0 -x bin/boot.hex -m r -b 0xC000 -e 0xFFFF
        done

