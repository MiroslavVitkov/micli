DEMONAME=demoRunEasy

all:
	avr-gcc $(CFLAGS) -L$(PROJ_DIR)/build -lcontrol -I$(PROJ_DIR)/include/ main.c -c -o $(PROJ_DIR)/build/$(DEMONAME).o
	avr-gcc $(LDFLAGS) -L$(PROJ_DIR)/build -lcontrol -I$(PROJ_DIR)/include/ \
 $(PROJ_DIR)/build/$(DEMONAME).o  -lcontrol -o $(PROJ_DIR)/build/$(DEMONAME).out
	avr-objcopy -j .text -j .data -O ihex  $(PROJ_DIR)/build/$(DEMONAME).out ../../build/$(DEMONAME).hex
