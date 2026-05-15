#include "MicroBit.h"
#include "acc.h"

MicroBit device;

int main()
{
    device.init();
    acc_test(device);
}
