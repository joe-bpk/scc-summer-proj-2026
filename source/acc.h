#ifndef ACC_H
#define ACC_H

#include "MicroBit.h"

/// exposed global device instance so event handlers can access it
extern MicroBit device;

void onButtonA(MicroBitEvent e);
void onButtonB(MicroBitEvent e);
void onAccelerometerData(MicroBitEvent e);

void setup_logging_events();


#endif  // ACC_H


