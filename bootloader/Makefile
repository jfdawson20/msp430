INC             = inc

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 

AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug -disable-simplify-libcalls

LOPS = -Wall -m32 -emit-llvm 

HLOPS = -Wall -emit-llvm 

LLCOPS = -march=msp430


all : boot.hex

clean :
	rm -f lib/*.o
	rm -f bin/*.elf
	rm -f bin/*.list
	rm -f bin/*.hex

lib/reset.o : src/reset.asm 
	msp430-as $(AOPS) src/reset.asm -o lib/reset.o

lib/vectortable.o : src/vectortable.asm
	msp430-as $(AOPS) src/vectortable.asm -o lib/vectortable.o

lib/boot.o : src/boot.c 
	msp430-gcc $(COPS) -c src/boot.c -o lib/boot.o

boot.elf : ld/memmap lib/boot.o lib/vectortable.o	lib/reset.o
	msp430-ld -T ld/memmap lib/boot.o lib/vectortable.o lib/reset.o -o bin/boot.elf

boot.hex : boot.elf
	msp430-objdump -D bin/boot.elf > bin/boot.list
	msp430-objcopy -O ihex bin/boot.elf bin/boot.hex

