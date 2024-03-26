#include <MATRIX7219.h>

#define DIN 11
#define CS 12
#define CLK 13

uint8_t EYE[] = {
    0b00000000,
    0b00111100,
    0b01110010,
    0b01110010,
    0b01111110,
    0b01111110,
    0b00111100,
    0b00000000
};

MATRIX7219 screen = MATRIX7219(DIN, CS, CLK, 2);

void print_matrix(uint8_t image[8], uint8_t display = 1) {
    for (int i = 1; i <= 8; i++) {
        screen.setRow(i, image[i-1], display);
    }
}

void blink_eyes() {
    for (int i = 1; i <= 4; i++) {
        screen.setRow(i, 0, 1);
        screen.setRow(8-i+1, 0, 1);
        screen.setRow(i, 0, 2);
        screen.setRow(8-i+1, 0, 2);
        delay(30);
    }
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i-1], 1);
        screen.setRow(8-i+1, EYE[8-i], 1);
        screen.setRow(i, EYE[i-1], 2);
        screen.setRow(8-i+1, EYE[8-i], 2);
        delay(30);
    }
}

void setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
    print_matrix(EYE, 1);
    print_matrix(EYE, 2);
}

void loop() {
    blink_eyes();
    delay(2500);
    // screen.clear(); 
    // for (int j = 1; j<=8; j++) {
        // for (int i = 0; i < 256; i++) {
            // screen.setRow(j, i, 1);
            // delay(10);
        // }
        // for (int i = 0; i < 256; i++) {
            // screen.setRow(j, i, 2);
            // delay(10);
        // }
    // }
}
