PROJNAME        = micli
UC              = atmega8
HEXFORMAT       = binary

LDFLAGS         = -lm -lc -Wall -mmcu=$(UC)
CFLAGS          = -fpack-struct -Os -mcall-prologues -mmcu=$(UC)
CFLAGS         += -finline-functions --std=c11
CFLAGS         += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors

all:
	# Compile.
	avr-gcc $(CFLAGS) commands.c -c -o build/commands.o

	# Link.
	avr-gcc $(LDFLAGS) $(LDFLAGS) build/commands.o -o build/$(PROJNAME).elf
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).elf build/$(PROJNAME).bin

	# Report.
	avr-size -B build/$(PROJNAME).elf

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
