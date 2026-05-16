#include "acc.h"

/// state variable to track recording state
/// (static restricts it to this file)
static int recordingEnabled = 0;

void onButtonA(MicroBitEvent e)
{
    recordingEnabled = !recordingEnabled;

    // Provide visual feedback
    if (recordingEnabled) {
        device.display.print("R");
    } else {
        device.display.print("S");
    }
}

void onButtonB(MicroBitEvent e)
{
    device.log.clear(true);

    device.display.print("C");
    device.sleep(500);

    if (recordingEnabled) {
        device.display.print("R");
    } else {
        device.display.clear();
    }
}

void onAccelerometerData(MicroBitEvent e)
{
    if (recordingEnabled) {
        int x = device.accelerometer.getX();
        int y = device.accelerometer.getY();
        int z = device.accelerometer.getZ();

        device.log.beginRow();
        device.log.logData("X", ManagedString(x));
        device.log.logData("Y", ManagedString(y));
        device.log.logData("Z", ManagedString(z));
        device.log.endRow();
    }
}

void setup_logging_events()
{
    /// wake up accelerometer this fixed the bug where no data is recorded
    device.accelerometer.getX();

    // register all event listeners
    device.messageBus.listen(MICROBIT_ID_BUTTON_A,
                             MICROBIT_BUTTON_EVT_CLICK,
                             onButtonA);
    device.messageBus.listen(MICROBIT_ID_BUTTON_B,
                             MICROBIT_BUTTON_EVT_CLICK,
                             onButtonB);
    device.messageBus.listen(MICROBIT_ID_ACCELEROMETER,
                             MICROBIT_ACCELEROMETER_EVT_DATA_UPDATE,
                             onAccelerometerData);
}
