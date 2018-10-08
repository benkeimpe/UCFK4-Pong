# File:   Makefile
# Author: M. P. Hayes, UCECE
# Date:   12 Sep 2010
# Descr:  Makefile for lab2-ex5

# Definitions.
CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g -I. -I./drivers/avr -I./utils -I./drivers
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm


# Default target.
all: game.out


# Compile: create object files from C source files.
game.o: game.c drivers/avr/system.h utils/tinygl.h paddle.h utils/pacer.h utils/font.h welcome.h drivers/button.h
	$(CC) -c $(CFLAGS) $< -o $@

paddle.o: paddle.c drivers/avr/system.h utils/tinygl.h drivers/navswitch.h utils/tinygl.h
	$(CC) -c $(CFLAGS) $< -o $@

welcome.o: welcome.c utils/tinygl.h welcome.h

pio.o: drivers/avr/pio.c drivers/avr/pio.h drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

system.o: drivers/avr/system.c drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

display.o: drivers/display.c drivers/avr/system.h drivers/display.h drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

ledmat.o: drivers/ledmat.c drivers/avr/pio.h drivers/avr/system.h drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

font.o: utils/font.c drivers/avr/system.h utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

navswitch.o: drivers/navswitch.c drivers/avr/delay.h drivers/avr/pio.h drivers/avr/system.h drivers/navswitch.h
	$(CC) -c $(CFLAGS) $< -o $@

tinygl.o: utils/tinygl.c drivers/avr/system.h drivers/display.h utils/font.h utils/tinygl.h
	$(CC) -c $(CFLAGS) $< -o $@

pacer.o: utils/pacer.c drivers/avr/system.h drivers/avr/timer.h utils/pacer.h
	$(CC) -c $(CFLAGS) $< -o $@

timer.o: drivers/avr/timer.c drivers/avr/system.h drivers/avr/timer.h
	$(CC) -c $(CFLAGS) $< -o $@

button.o: drivers/button.c drivers/button.h
	$(CC) -c $(CFLAGS) $< -o $@




# Link: create ELF output file from object files.
game.out: game.o paddle.o system.o display.o ledmat.o font.o tinygl.o pio.o navswitch.o pacer.o timer.o welcome.o button.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@


# Target: clean project.
.PHONY: clean
clean:
	-$(DEL) *.o *.out *.hex


# Target: program project.
.PHONY: program
program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash game.hex; dfu-programmer atmega32u2 start