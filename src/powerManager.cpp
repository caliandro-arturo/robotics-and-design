/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include "pins.hpp"
#include "powerManager.hpp"
#include <LowPower.h>

volatile power power_status = OFF;

void init_power() {
    attachInterrupt(digitalPinToInterrupt(POWER_BTN), power_check, CHANGE);
    pinMode(RELAY_CTRL, INPUT);
    power_check();
}

void power_check() {
    if (digitalRead(POWER_BTN) == LOW && power_status == ON) {
        power_status == OFF;
    }
}

// Watch out: operations performed here should be treated as a stack:
// first disabled == last re-enabled.
void shutdown() {
    // Turn off
    // TODO disable every interrupt EXCEPT the power button one
    digitalWrite(RELAY_CTRL, HIGH);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  // Execution frozen here
    // Turn on
    digitalWrite(RELAY_CTRL, LOW);
    // TODO re-enable the disabled interrupts
    power_status = ON;
}
