#ifndef ACC_H
#define ACC_H

#include "MicroBit.h"

extern MicroBit device;

// Phase 3 Parameters
#define WINDOW_SIZE 5
#define THRESHOLD_THETA 450 // Magnitude threshold in milli-g
#define BASELINE_MEAN 1025   // Subtracted baseline mean

void onButtonA(MicroBitEvent e);
void onButtonB(MicroBitEvent e);
void onAccelerometerData(MicroBitEvent e);
void setup_logging_events();

// ARM Assembly Optimized Average Function
extern "C" int asm_average(const int* window, int size);

#endif // ACC_H
