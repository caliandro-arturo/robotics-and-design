#include "timer.hpp"

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D

const uint8_t hi[] = {
    0b01110100,
    0b00010000,
    0b00100101,
    0b00010011
};

volatile bool toUpdate;

Timer::Timer(uint8_t displayCLK, uint8_t displayDIO)
    : timerOn(false),
      blink(false),
      colon(true),
      hours(0),
      minutes(0),
      digits({ 0, 0b10111111, 0b00111111, 0b00111111 }),
      display(displayCLK, displayDIO) {
    display.clear();
    display.setBrightness(1);
    display.setSegments(digits);
}

void Timer::setDisplay(bool value) {
    display.setBrightness(1, value);
}

void Timer::setBlink(bool value) {
}

void Timer::increaseMinutes(int8_t direction) {
    minutes += direction * 5;
    if (minutes >= 2 * 60) {
        hours += minutes / (2 * 60);
        minutes = 0;
    } else if (minutes <= 0) {
        if (hours == 0) {
            minutes = 0;
        } else {
            hours--;
            minutes = 2 * 55;
        }
    }
    digits[1] = display.encodeDigit(hours) | (colon << 7);
    digits[2] = display.encodeDigit((minutes >> 1) / 10);
    digits[3] = display.encodeDigit((minutes >> 1) % 10);
    display.setSegments(digits);
}

void Timer::setTimerOn(bool value) {
}

void Timer::resetTimer() {
}

int Timer::runCoroutine() {
    return 0;
}

Timer::~Timer() {
}
