#include "MicroBit.h"
#include "acc.h"

MicroBit device;

int main()
{
    device.init();

    setup_logging_events();

    while (1) {
        device.sleep(1000);
    }
}
