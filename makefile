PROJNAME        = micli
UC              = atmega168
HEXFORMAT       = binary
BOOTLOAD        = 0x3E00 # byte address, start of bootlaoder

CONTROLINC      =../libcontrol/include/
CONTROLDIR      =../libcontrol/build/
CONTROLLIB      =control

LDFLAGS         = -lm -lc -Wall -mmcu=$(UC) -L$(CONTROLDIR) -l$(CONTROLLIB)
CFLAGS          = -fpack-struct -Os -mcall-prologues -mmcu=$(UC) -I$(CONTROLINC)
CFLAGS         += -finline-functions --std=c11
CFLAGS         += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors -Wpedantic
CFLAGS         += -DBOOTLOAD=$(BOOTLOAD)

all:
	# Compile.
	avr-gcc $(CFLAGS) src/main.c -c -o  build/main.o
	avr-gcc $(CFLAGS) src/commands.c -c -o build/commands.o
	avr-gcc $(CFLAGS) src/pid_tune.c -c -o build/pid_tune.o
	avr-gcc $(CFLAGS) src/usart.c -c -o build/usart.o
	avr-gcc $(CFLAGS) src/zcd.c -c -o build/zcd.o

	avr-gcc $(CFLAGS) libs/crc/crc8.c -c -o build/crc8.o
	avr-gcc $(CFLAGS) libs/ds18x20/ds18x20.c -c -o build/ds18x20.o
	avr-gcc $(CFLAGS) libs/onewire/onewire.c -c -o build/onewire.o

	# Link.
	avr-gcc $(LDFLAGS) build/main.o build/commands.o build/pid_tune.o build/usart.o build/zcd.o build/crc8.o build/ds18x20.o build/onewire.o -l$(CONTROLLIB) -o build/$(PROJNAME).elf
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).elf build/$(PROJNAME).bin

	# Report.
	avr-size -B build/$(PROJNAME).elf

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
