#include "MicroBit.h"
#include "acc.h"

MicroBit device;

int main()
{
    device.init();

    setup_logging_events();

    while (1) {
        /// wake up accelerometer this fixed the bug where no data is recorded
        device.accelerometer.getX();
        device.sleep(100);
    }
}
