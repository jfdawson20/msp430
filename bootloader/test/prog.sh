#!/bin/bash
make clean 
make  
mspdebug rf2500 "prog bin/boot.hex"
minicom -s reflow 
