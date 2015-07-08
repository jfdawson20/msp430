# makfile configuration
NAME            = reflow
OBJECTS         = ${LIB}/main.o ${LIB}/misc.o ${LIB}/uart.o ${LIB}/adc.o ${LIB}/PIDControl.o ${LIB}/timer.o 
CPU             = msp430g2553
LIB             = lib
INC             = include 
BIN             = bin
SRC             = src

COPS = -Wall -O2 -nostartfiles -ffreestanding 
AOPS = --warn --fatal-warnings

#switch the compiler (for the internal make rules)
CC              = msp430-gcc

#all should be the first target. it's built when make is run without args
all: ${NAME}.elf ${NAME}.a43 ${NAME}.lst

#confgigure the next line if you want to use the serial download
download: download-jtag
#download: download-bsl

#additional rules for files
${NAME}.elf: ${BIN} ${LIB} ${OBJECTS}
	 msp430-gcc -mmcu=msp430g2553 -T ld/msp430.x lib/main.o lib/misc.o lib/uart.o lib/adc.o lib/PIDControl.o lib/timer.o -o bin/reflow.elf

${NAME}.a43: ${BIN}/${NAME}.elf
	msp430-objcopy -O ihex $^ ${BIN}/$@

${NAME}.lst: ${BIN}/${NAME}.elf
	msp430-objdump -dSt $^ >${BIN}/$@

clean:
	rm -f lib/*.o
	rm -f bin/*.elf
	rm -f bin/*.lst
	rm -f bin/*.a43

#backup archive
dist:
	tar czf dist.tgz *.c *.h *.txt makefile

#dummy target as dependecy if something has to be build everytime
FORCE:

#project dependencies
${BIN}: 
	mkdir bin

${LIB}:
	mkdir lib
	
${LIB}/main.o: ${SRC}/main.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/main.c 

${LIB}/misc.o: ${SRC}/misc.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/misc.c 

${LIB}/uart.o: ${SRC}/uart.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/uart.c 

${LIB}/adc.o: ${SRC}/adc.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/adc.c 

${LIB}/PIDControl.o: ${SRC}/PIDControl.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/PIDControl.c 
	
${LIB}/timer.o: ${SRC}/timer.c 
	msp430-gcc $(COPS) -iquote ${INC} -c -o $@ ${SRC}/timer.c 

