DEMONAME=hardwareTest

all:
	#compile
	avr-gcc $(CFLAGS) -I$(PROJ_DIR)/include/ -Ilibs/crc8/ -Ilibs/ds18x20/ -Ilibs/onewire/ $(DEMONAME).c -c -o $(PROJ_DIR)/build/$(DEMONAME).o
	avr-gcc $(CFLAGS) -I$(PROJ_DIR)/include/ usart.c -c -o $(PROJ_DIR)/build/usart.o
	#avr-gcc $(CFLAGS) -L$(PROJ_DIR)/build -lcontrol -I$(PROJ_DIR)/include/ libs/crc/crc8.c -c -o $(PROJ_DIR)/build/crc8.o
	#avr-gcc $(CFLAGS) -L$(PROJ_DIR)/build -lcontrol -I$(PROJ_DIR)/include/ -Ilibs/ds18x20 -Ilibs/onewire -Ilibs/crc \
		# libs/ds18x20/ds18x20.c -c -o $(PROJ_DIR)/build/ds18x20.o
	#avr-gcc $(CFLAGS) -L$(PROJ_DIR)/build -lcontrol -I$(PROJ_DIR)/include/ libs/onewire/onewire.c -c -o $(PROJ_DIR)/build/onewire.o

	#link
	avr-gcc $(LDFLAGS) -L$(PROJ_DIR)/build -I$(PROJ_DIR)/include/ $(PROJ_DIR)/build/$(DEMONAME).o\
		 $(PROJ_DIR)/build/usart.o -lcontrol -o $(PROJ_DIR)/build/$(DEMONAME).out 
	avr-objcopy -j .text -j .data -O ihex  $(PROJ_DIR)/build/$(DEMONAME).out ../../build/$(DEMONAME).hex

.PHONY: upload fuses
upload:
	sudo avrdude -p m168 -c usbasp -e -U flash:w:../../build/$(DEMONAME).hex 

fuses:
	sudo avrdude -p m168 -c usbasp -t #-U lfuse:w:fuses
