.PHONY: all build flash monitor clean

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
