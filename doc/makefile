SRC = a.c b.c
VPATH += $(PROJ_DIR):$(PROJ_DIR)/include:$(PROJ_DIR)/include/atmel
CFLAGS +=  -I$(PROJ_DIR)/include -I$(PROJ_DIR)

OBJC = $(patsubst %.c, $(PROJ_DIR)/build/%.o, $(filter %.c, $(SRC)))
OBJA = $(patsubst %.s, $(PROJ_DIR)/build/%.o, $(filter %.s, $(SRC)))
DISASSM = $(patsubst %.c, $(PROJ_DIR)/build/%.S, $(filter %.c, $(SRC)))
DFILES = $(patsubst %.c, $(PROJ_DIR)/build/%.d, $(SRC))
OBJ = $(OBJC) $(OBJA)

all: $(OBJ)

disasm: $(DISASSM)

-include $(DFILES)

# .c -> .o
$(OBJC): $(PROJ_DIR)/build/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
#	$(CC) -MM $(CFLAGS) $*.c > $*.d

# .s -> .o
$(OBJA): $(PROJ_DIR)/build/%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

# .c -> .S	#disassembly
$(DISASSM): $(PROJ_DIR)/build/%.S: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# .c -> .d	#dependancy lists
$(DFILES): $(PROJ_DIR)/build/%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(REMOVE) $@.$$$$
