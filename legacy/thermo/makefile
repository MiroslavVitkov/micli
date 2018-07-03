PROJNAME=thermo
UC=atmega8
LDFLAGS = -lm -lc -Wall -mmcu=$(UC)
HEXFORMAT = binary #ihex
CONTROLINC=../libcontrol/include/
CONTROLDIR=../libcontrol/build/
CONTROLLIB=control
CFLAGS = -fpack-struct -Wall -Os -mcall-prologues -mmcu=$(UC) -Winline -finline-functions -Wstrict-prototypes --std=c99 -Winline -Wno-main -I$(CONTROLINC)

all:
	#compile
	avr-gcc $(CFLAGS) -I$(CONTROLINC) -Ilibs/crc8/ -Ilibs/ds18x20/ -Ilibs/onewire/ $(PROJNAME).c -c -o build/$(PROJNAME).o
	avr-gcc $(CFLAGS) usart.c -c -o build/usart.o
	avr-gcc $(CFLAGS) -Lbuild -Ilibs/crc8 libs/crc/crc8.c -c -o build/crc8.o
	avr-gcc $(CFLAGS) -Lbuild -Ilibs/ds18x20 -Ilibs/onewire -Ilibs/crc libs/ds18x20/ds18x20.c -c -o build/ds18x20.o
	avr-gcc $(CFLAGS) -Lbuild -Ilibs/onewire libs/onewire/onewire.c -c -o build/onewire.o

	#link
	avr-gcc $(LDFLAGS) -L$(CONTROLDIR) -Lbuild build/$(PROJNAME).o build/usart.o build/ds18x20.o build/onewire.o build/crc8.o -o build/$(PROJNAME).elf -l$(CONTROLLIB) 
	avr-objcopy -j .text -j .data -O $(HEXFORMAT)  build/$(PROJNAME).elf build/$(PROJNAME).bin

	# Report
	avr-size -B build/$(PROJNAME).elf

.PHONY: upload fuses clean
upload:
	sudo avrdude -p $(UC) -c usbasp -e -U flash:w:build/$(PROJNAME).hex

fuses:
	sudo avrdude -p $(UC) -c usbasp -U lfuse:w:0xE4:m -U hfuse:w:0xD9:m
# Default for the atmega8 is lfuse:e1, hfuse:d9
# Low fuse for 8MHz clock: E4
# High fuse with 1024 words for bootloader, start at app start: D9

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
