.PHONY: all build flash monitor clean

ANALYSIS_SRC = analysis_source/main.cpp
A_CC = g++
A_BUILD = a_build
A_PROG = $(A_BUILD)/analysis

$(A_BUILD):
	mkdir -p $(A_BUILD)

analysis_build:	$(A_BUILD)
	$(A_CC)	 $(ANALYSIS_SRC) -o $(A_PROG)

analysis_run: analysis_build
	./$(A_PROG)


all: build

build:
	python3 build.py

flash: build
	udisksctl mount -b /dev/disk/by-label/MICROBIT
	cp MICROBIT.hex /run/media/${USER}/MICROBIT

monitor: flash
	cat /dev/ttyACM0

clean:
	rm -rf build/
