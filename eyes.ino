
#define DIN 11
#define CS 12
#define CLK 13

#define PROXIMITY_IR 2

using namespace ace_routine;

enum Mood {NORMAL, HAPPY, SAD, ANGRY, PISSED };

volatile Mood mood;
//volatile bool interruptOccurred = false;
int oldProximity;


uint8_t UP_RIGHT_EYE[] = {
    0b00000000,
    0b00111100,
    0b01110010,
    0b01110010,
    0b01111110,
    0b01111110,
    0b00111100,
    0b00000000
};

uint8_t  ANGRY_EYE[] = {
    0b00000000,
    0b00100000,
    0b01110000,
    0b01111000,
    0b01100100,
    0b01100110,
    0b00111100,
    0b00000000
};

uint8_t  FRONT_EYE[] = {
    0b00000000,
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111100,
    0b00000000
};

uint8_t  HAPPY_EYE[] = {
    0b00000000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b01100110,
    0b11000011,
    0b10000001,
    0b00000000
};

uint8_t  PISSED_EYE[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000
};

MATRIX7219 screen = MATRIX7219(DIN, CS, CLK, 2);

void print_matrix(uint8_t image[8], uint8_t display = 1) {
    for (int i = 1; i <= 8; i++) {
        screen.setRow(i, image[i - 1], display);
    }
}

uint8_t reverse(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

void close_eyes() {
    for (int i = 1; i <= 4; i++) {
        screen.setRow(i, 0, 1);
        screen.setRow(8 - i + 1, 0, 1);
        screen.setRow(i, 0, 2);
        screen.setRow(8 - i + 1, 0, 2);
        delay(30);
    }
}

void side_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, EYE[i - 1], 2);
        screen.setRow(8 - i + 1, EYE[8 - i], 2);
        delay(30);
    }
}

void front_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, reverse(EYE[i - 1]), 2);
        screen.setRow(8 - i + 1, reverse(EYE[8 - i]), 2);
        delay(30);
    }
}

#define PROXIMITY_IR 2

void eyes_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
    print_matrix(UP_RIGHT_EYE, 1);
    print_matrix(UP_RIGHT_EYE, 2);
    pinMode(PROXIMITY_IR, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(PROXIMITY_IR), proximityInterrupt, CHANGE); 
    mood = NORMAL;
    Serial.begin(9600);
}

void proximityInterrupt() {
    int proximity = digitalRead(PROXIMITY_IR);
    if (proximity == LOW) {
        mood = ANGRY;
        //interruptOccurred = true; 
    } else {
        mood = NORMAL;
        //interruptOccurred = true; 
    }
}


void eyes_loop() {
    if (mood == ANGRY) {
        close_eyes();
        delay(50);
        front_eyes(ANGRY_EYE);
        delay(1500);
    } else if (mood == NORMAL) {
        close_eyes();
        delay(50);
        side_eyes(UP_RIGHT_EYE);
        delay(1500);
    }
}

