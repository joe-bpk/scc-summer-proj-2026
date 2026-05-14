#!/usr/bin/env bash

python3 build.py
udisksctl mount -b /dev/disk/by-label/MICROBIT
cp MICROBIT.hex /run/media/${USER}/MICROBIT
picocom /dev/ttyACM0 -b 115200
