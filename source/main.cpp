#include "MicroBit.h"

MicroBit uBit;

int main()
{
    uBit.init();
    while(1)
    {
        uBit.serial.printf("Acc [X:%d][Y:%d][Z:%d]\r\n",
            uBit.accelerometer.getX(),
            uBit.accelerometer.getY(),
            uBit.accelerometer.getZ());
        uBit.sleep(1000);
    }
}
