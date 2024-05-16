/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include "pins.hpp"
#include "powerManager.hpp"
#include <MATRIX7219.h>
#include <LowPower.h>
#include <TM1637Display.h>

volatile power power_status = ON;

extern MATRIX7219 screen;
extern TM1637Display display;

extern void eye_setup();
extern void reset_timer();

void init_power() {
    pinMode(POWER_BTN, INPUT);
    attachInterrupt(digitalPinToInterrupt(POWER_BTN), power_check, CHANGE);
    pinMode(RELAY_CTRL, OUTPUT);
    power_check();
}

void power_check() {
    if (digitalRead(POWER_BTN) == LOW && power_status == ON) {
        power_status = OFF;
    }
}

// Watch out: operations performed here should be treated as a stack:
// first disabled == last re-enabled.
void shutdown() {
    // Turn off
    noInterrupts();
    digitalWrite(EYE_CLK, LOW);
    digitalWrite(EYE_CS, LOW);
    digitalWrite(EYE_DIN, LOW);
    PCICR &= ~bit(digitalPinToPCICRbit(ENCODER_CLK));
    digitalWrite(RELAY_CTRL, HIGH);
    interrupts();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  // Execution frozen here
    // Turn on
    digitalWrite(RELAY_CTRL, LOW);
    PCICR |= bit(digitalPinToPCICRbit(ENCODER_CLK));
    delay(5);   // Workaround to ensure flawless communication with displays
    eye_setup();
    reset_timer();
    power_status = ON;
}
