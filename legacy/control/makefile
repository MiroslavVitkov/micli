#Library-level makefile. Should not be called directly.

#TODO: MAKELEVEL 1 when first called, 2 when makefile calls itself
ifeq (1, $(MAKELEVEL))	#make counts from 0
	#first entrance in the file
	#indicete that everithung is ok
#	OK=1
else
	ifeq (2, $(MAKELEVEL))
		#self recursive call
		#check that everithing is ok
		ifneq (OK, 1)
#			$(error This makefile cannot be called directly. Please use the top level makefile. $(MAKELEVEL))
		endif
	else
#	$(error This makefile cannot be called directly. Please use the top level makefile. $(MAKELEVEL))
	endif
endif

#settings
export
PROJ = control
MODULES = pid step lin onoff 
DEMOS = #hardwareTest #RunBare #CalibrateAndRun RunEasy 
VPATH += src:include:build

PROJNAME = lib$(PROJ).a
PROJ_DIR = $(CURDIR)

.PHONY: all disasm clean archive demos clean_intermediate
all:
	$(foreach var, $(MODULES), cd $(PROJ_DIR)/src/$(var) && $(MAKE) all;)
	$(MAKE) archive
	$(MAKE) demos
	$(MAKE) clean_intermediate

disasm: $(DISASSM)
	$(foreach var, $(MODULES), cd $(PROJ_DIR)/src/$(var) && $(MAKE) disasm;)	
	$(MAKE) clean_useless

#config: #TODO

clean:
	$(REMOVE) build/*

clean_intermediate:
	rm -f build/*.out build/*.d build/demo*.o

archive:
	$(ARCHIVE) rcs build/$(PROJNAME) $(wildcard build/*.o)

demos:
	$(foreach var, $(DEMOS), cd $(PROJ_DIR)/demos/$(var) && $(MAKE) all;)
