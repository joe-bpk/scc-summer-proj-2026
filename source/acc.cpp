#include "acc.h"

#include <cmath>

static int recordingEnabled = 0;
static int jumpCount        = 0;

static int slidingWindow[WINDOW_SIZE] = { 0 };

static int  prev_sma_y        = 0;
static int  local_extremum    = 0;
static bool searching_for_max = true;
static int  last_crossing_val = 0;

const uint8_t arrow_up_buffer[]   = { 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1,
                                      0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
const uint8_t arrow_down_buffer[] = { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1,
                                      0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0 };

MicroBitImage arrowUp(5, 5, arrow_up_buffer);
MicroBitImage arrowDown(5, 5, arrow_down_buffer);

extern "C" int asm_average(const int* window, int size)
{
    int sum = 0;
    __asm__ volatile(
        "MOV r2, #0\n\t"  // Accumulator (sum = 0)
        "MOV r3, #0\n\t"  // Loop counter (i = 0)
        "loop_start:\n\t"
        "CMP r3, %[size]\n\t"  // Check if i == size
        "BGE loop_end\n\t"
        "LDR r4, [%[window], r3, LSL #2]\n\t"  // Load window[i]
        "ADD r2, r2, r4\n\t"                   // sum += window[i]
        "ADD r3, r3, #1\n\t"                   // i++
        "B loop_start\n\t"
        "loop_end:\n\t"
        "SDIV %[sum], r2, %[size]\n\t"  // sum = sum / size
        : [sum] "=r"(sum)
        : [window] "r"(window), [size] "r"(size)
        : "r2", "r3", "r4", "cc");
    return sum;
}

void onButtonA(MicroBitEvent e)
{
    recordingEnabled = !recordingEnabled;
    if (!recordingEnabled) {
        device.display.scroll(jumpCount);
    } else {
        device.display.print("R");
    }
}

void onButtonB(MicroBitEvent e)
{
    jumpCount         = 0;
    local_extremum    = 0;
    last_crossing_val = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) slidingWindow[i] = 0;

    device.log.clear(true);
    device.display.print("C");
    device.sleep(400);
    device.display.clear();
}

void onAccelerometerData(MicroBitEvent e)
{
    int raw_y      = device.accelerometer.getY();
    int centered_y = raw_y - BASELINE_MEAN;

    for (int i = 0; i < WINDOW_SIZE - 1; i++) {
        slidingWindow[i] = slidingWindow[i + 1];
    }
    slidingWindow[WINDOW_SIZE - 1] = centered_y;

    int current_sma_y = asm_average(slidingWindow, WINDOW_SIZE);

    if (prev_sma_y <= 0 && current_sma_y > 0) {
        device.display.print(arrowUp);

        int magnitude = std::abs(local_extremum - current_sma_y);
        if (magnitude > THRESHOLD_THETA) {
            jumpCount++;
        }
        local_extremum = current_sma_y;
    } else if (prev_sma_y >= 0 && current_sma_y < 0) {
        device.display.print(arrowDown);

        int magnitude = std::abs(local_extremum - current_sma_y);
        if (magnitude > THRESHOLD_THETA) {
        }
        local_extremum = current_sma_y;
    } else if (std::abs(current_sma_y) < 50) {
        device.display.clear();
    }

    if (current_sma_y > local_extremum && current_sma_y > 0) {
        local_extremum = current_sma_y;
    } else if (current_sma_y < local_extremum && current_sma_y < 0) {
        local_extremum = current_sma_y;
    }

    if (recordingEnabled) {
        device.log.beginRow();
        device.log.setTimeStamp(TimeStampFormat::Milliseconds);
        device.log.logData("X", ManagedString(device.accelerometer.getX()));
        device.log.logData("Y", ManagedString(raw_y));
        device.log.logData("Z", ManagedString(device.accelerometer.getZ()));
        device.log.endRow();
    }

    prev_sma_y = current_sma_y;
}

void setup_logging_events()
{
    device.accelerometer.getX();  // Wake up sensor

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
