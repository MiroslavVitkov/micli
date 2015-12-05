PROJNAME        = micli
UC              = atmega168
HEXFORMAT       = binary
BOOTLOAD        = 0x3E00 # byte address, start of bootlaoder

LDFLAGS         = -lm -lc -Wall -mmcu=$(UC)
CFLAGS          = -fpack-struct -Os -mcall-prologues -mmcu=$(UC)
CFLAGS         += -finline-functions --std=c11
CFLAGS         += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors -Wpedantic
CFLAGS         += -DBOOTLOAD=$(BOOTLOAD)

all:
	# Compile.
	avr-gcc $(CFLAGS) src/main.c -c -o  build/main.o
	avr-gcc $(CFLAGS) src/usart.c -c -o build/usart.o
	avr-gcc $(CFLAGS) src/commands.c -c -o build/commands.o

	# Link.
	avr-gcc $(LDFLAGS) $(LDFLAGS) build/main.o build/usart.o build/commands.o -o build/$(PROJNAME).elf
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).elf build/$(PROJNAME).bin

	# Report.
	avr-size -B build/$(PROJNAME).elf

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
