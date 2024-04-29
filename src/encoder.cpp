#include "encoder.hpp"
#include "pins.hpp"
#include "timer.hpp"

extern Timer *timer;
volatile uint8_t rotaryState;
volatile bool pushState;
volatile bool halfTurn;

/** Reads the current state of the encoder signals CLK and DT, and stores
 * them in a single integer using the two LSBs.
 *
 * E.g.:
 * CLK = 1, DT = 0 ---> 0b010
 * CLK = 0, DT = 1 ---> 0b001
*/
uint8_t read() {
    // Direct port read is made to
    uint8_t pins = ENCODER_PINS;
    return (!!(pins & digitalPinToBitMask(ENCODER_CLK)) * 2
            | !!(pins & digitalPinToBitMask(ENCODER_DT)));
}

void encoderInit() {
    pinMode(ENCODER_CLK, INPUT);
    pinMode(ENCODER_DT, INPUT);
    pinMode(ENCODER_SW, INPUT_PULLUP);
    pushState = HIGH;
    halfTurn = false;
    // Enable pin change interrupts on the encoder port (assuming every
    // pin is plugged on the same I/O port, as required in pins.hpp)
    uint8_t PCICRbit = digitalPinToPCICRbit(ENCODER_CLK);
    PCICR |= bit(PCICRbit);
    ENCODER_PCMSK |= bit(digitalPinToPCMSKbit(ENCODER_CLK))
                     | bit(digitalPinToPCMSKbit(ENCODER_DT))
                     | bit(digitalPinToPCMSKbit(ENCODER_SW));
    noInterrupts();
    rotaryState = read();
    // Hope that encoder state does not change here :3
    interrupts();
}

void onButtonPress() {
}

void onButtonRelease() {
}

void onEncoderRotate(int8_t direction) {
    timer->increaseMinutes(direction);
}

/** Interrupt service routine dedicated to the encoder */
ISR(ENCODER_VEC) {
    // encoder pushbutton
    if ((PINB & bit(PB4)) == 0 && pushState == HIGH) {
        pushState = LOW;
        onButtonPress();
        return;
    }
    if ((PINB & bit(PB4)) != 0 && pushState == LOW) {
        pushState = HIGH;
        onButtonRelease();
        return;
    }
    // rotary encoder
    //
    // The debouncing is made via software, inspired by the algorithm
    // made by Mark Pinteric. Its explaination and code can be found
    // here: https://www.pinteric.com/rotary.html .
    //
    uint8_t newState = read();
    if (newState == rotaryState)
        return;
    if ((newState ^ rotaryState) != 0b11) {
        switch ((rotaryState << 2) | newState) {
            // Clockwise rotation
            case 0b0010:
            case 0b1011:
            case 0b1101:
            case 0b0100:
                onEncoderRotate(1);
                break;
            // Counter clockwise rotation
            default:
                onEncoderRotate(-1);
        }
    }
    rotaryState = newState;
}