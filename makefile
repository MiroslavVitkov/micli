PROJNAME        = micli
UC              = atmega168
HEXFORMAT       = binary
BOOTLOAD        = 0x3E00 # byte address, start of bootlaoder

LDFLAGS         = -lm -lc -Wall -mmcu=$(UC)
LDFLAGS        += -Wl,-Map,build/$(PROJNAME).map
CFLAGS          = -fpack-struct -Os -mcall-prologues -mmcu=$(UC)
CFLAGS         += -finline-functions --std=c11
CFLAGS         += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors -Wpedantic
CFLAGS         += -DBOOTLOAD=$(BOOTLOAD)

all:
	# Compile.
	avr-gcc $(CFLAGS) src/main.c -c -o  build/main.o
	avr-gcc $(CFLAGS) src/clock.c -c -o build/clock.o
	avr-gcc $(CFLAGS) src/commands.c -c -o build/commands.o
	avr-gcc $(CFLAGS) src/pid_tune.c -c -o build/pid_tune.o
	avr-gcc $(CFLAGS) src/strings.c -c -o build/strings.o
	avr-gcc $(CFLAGS) src/usart.c -c -o build/usart.o
	avr-gcc $(CFLAGS) src/tempr.c -c -o build/tempr.o
	avr-gcc $(CFLAGS) src/zcd.c -c -o build/zcd.o

	avr-gcc $(CFLAGS) libs/crc/crc8.c -c -o build/crc8.o
	avr-gcc $(CFLAGS) libs/ds18x20/ds18x20.c -c -o build/ds18x20.o
	avr-gcc $(CFLAGS) libs/onewire/onewire.c -c -o build/onewire.o
	avr-gcc $(CFLAGS) libs/pid/pid.c -c -o build/pid.o

	# Link.
	avr-gcc $(LDFLAGS) build/main.o build/clock.o build/commands.o build/pid_tune.o build/strings.o build/usart.o build/tempr.o build/zcd.o build/crc8.o build/ds18x20.o build/onewire.o build/pid.o -o build/$(PROJNAME).elf
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).elf build/$(PROJNAME).bin

	# Report.
	avr-size -B build/$(PROJNAME).elf

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
