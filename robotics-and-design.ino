#include <AceRoutine.h>
#include "src/encoder.hpp"
#include "src/pins.hpp"
#include "src/timer.hpp"

Timer *timer;

void setup() {
    timer = new Timer(DISPLAY_CLK, DISPLAY_DIO);
    encoderInit();
}

void loop() {
}
