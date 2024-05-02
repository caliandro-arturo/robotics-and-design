#include "eyes.hpp"
#include "MATRIX7219.h"



MATRIX7219 screen = MATRIX7219(EYE_DIN, EYE_CS, EYE_CLK, 2);


Eyes::Eyes() {
    
    lastTimeEye = 0;
}


const uint8_t HAPPY_EYE[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100
};

const uint8_t ANGRY_EYE[8] = {
    B00111100,
    B01000010,
    B10000001,
    B10100101,
    B10011001,
    B10100101,
    B01000010,
    B00111100
};

const uint8_t FRONT_EYE[8] = {
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111
};

void Eyes::print_matrix(uint8_t image[8], uint8_t display) {
    for (int i = 1; i <= 8; i++) {
        screen.setRow(i, image[i - 1], display);
    }
}

uint8_t Eyes::reverse(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

void Eyes::close_eyes() {
    for (int i = 1; i <= 4; i++) {
        screen.setRow(i, 0, 1);
        screen.setRow(8 - i + 1, 0, 1);
        screen.setRow(i, 0, 2);
        screen.setRow(8 - i + 1, 0, 2);
        delay(30);
    }
}

void Eyes::side_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, EYE[i - 1], 2);
        screen.setRow(8 - i + 1, EYE[8 - i], 2);
        delay(30);
    }
}

void Eyes::front_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, reverse(EYE[i - 1]), 2);
        screen.setRow(8 - i + 1, reverse(EYE[8 - i]), 2);
        delay(30);
    }
}

void Eyes::assignEye(uint8_t EYE[8]) {
    for (int i = 0; i < 8; i++) {
        currentEye[i] = EYE[i];
    }
}


int Eyes::runCoroutine() {
  COROUTINE_LOOP() {
    close_eyes();
    COROUTINE_DELAY(50);
    front_eyes(currentEye);
  
    COROUTINE_DELAY(1500);
  }
}





void Eyes::blink_happy() {
    close_eyes();
    delay(50);
    front_eyes(HAPPY_EYE);
    delay(1000);
}

void Eyes::proximityInterrupt() {
    unsigned long timeNow = millis();
    int proximity = digitalRead(PROXIMITY_IR);
    if (proximity == LOW && mood == ANGRY && timeNow - lastTimeEye > DEBOUNCING_PERIOD) {
        mood = NORMAL;
        assignEye(FRONT_EYE);
        Serial.println("0");
        Serial.println("1");
    } else if (proximity == HIGH && mood == NORMAL && timeNow - lastTimeEye > DEBOUNCING_PERIOD) {
        mood = ANGRY;
        assignEye(ANGRY_EYE);
        Serial.println("1");
    }
    lastTimeEye = timeNow;
}

void Eyes::eye_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
    pinMode(PROXIMITY_IR, INPUT_PULLUP);
    mood = NORMAL;
    assignEye(FRONT_EYE);
    front_eyes(currentEye);
    //Serial.begin(9600);
}

