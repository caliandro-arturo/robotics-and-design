#include "src/arms.hpp"
#include "src/body.hpp"
#include "src/ears.hpp"
#include "src/head.hpp"
#include "src/pins.hpp"
#include "src/powerManager.hpp"
#include "src/servoCalibration.hpp"
#include "src/timer.hpp"

#include <AceRoutine.h>
#include <MATRIX7219.h>
#include <TM1637Display.h>

#define DEBOUNCING_PERIOD 100

using namespace ace_routine;

unsigned long lastTimeEye = 0UL;
int setMinute;
int setHour;
byte lastButtonState = HIGH;
bool isMinuteSet;
bool isHourSet;
int minute;
int hour;
volatile int encoderPos = 0;
int oldProximity;
bool phonePresent;
extern power power_status;

uint8_t currentEye[8];

enum Mood { NORMAL,
            HAPPY,
            SAD,
            ANGRY,
            PISSED };

volatile Mood mood;

enum Status { START,
              SET_MINUTES,
              SET_HOURS,
              PHONE_CHECK,
              TIMER_GOING,
              HAND_DETECTED,
              TIMER_FINISHED };

volatile Status status;



/*
*
*
* PHONE PRESENCE
*
*
*/


typedef struct {
        int pin;
        bool phoneIn;
        //RGB_Led rgbled;
} PhoneSlot;

PhoneSlot phoneSlot[1];


void init_phone_slots() {
    phoneSlot[0].pin = SLOT0;
    /*phoneSlot[1].pin = SLOT1;
  phoneSlot[2].pin = SLOT2;
  phoneSlot[3].pin = SLOT3;*/
    /* phonePresent = false;
  for(int i=0; i<1; i++){
    phoneSlot[i].phoneIn = false;
  }
  init_leds();*/
}

//may be the opposite
void check_phone() {
    int count = 0;
    //for(int i= 0; i<1; i++){
    if (digitalRead(phoneSlot[0].pin) == LOW) {  //detected
        phonePresent = true;
        phoneSlot[0].phoneIn = true;
        count++;

    } else {
        phonePresent = false;
        phoneSlot[0].phoneIn = false;
    }
    // }
    // if (count>0)
    //  phonePresent = true;
}



/*
*
*
* CLOCK ENCODER
*
*/


TM1637Display display(TIMER_CLK, TIMER_DIO);


volatile int counter;
int lastEnA = LOW;
unsigned long lastTime = 0UL;

unsigned long previousMillis = 0;
unsigned long interval = 1000;

volatile int lastEncoded = 0;
unsigned long currentMillis;

void reset_encoder() {
    encoderPos = 0;
}

void increment_hours() {
    hour = (encoderPos % 30) / 10;
}

void increment_minutes() {
    minute = (encoderPos % 300) / 5;
}


void increment_status() {
    unsigned long timeNow = millis();
    if (timeNow - lastTime > DEBOUNCING_PERIOD) {
        if (status == START)
            status = SET_MINUTES;
        else if (status == SET_MINUTES) {
            status = SET_HOURS;
            //setMinute = minute;
        }
        lastTime = timeNow;
    }
}

void encoderISR() {
    int MSB = digitalRead(ENCODER_CLK);
    int LSB = digitalRead(ENCODER_DT);

    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderPos++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderPos--;
    }

    lastEncoded = encoded;
}


void clock_setup() {
    counter = 0;
    display.setBrightness(7);
    display.clear();
    minute = 0;
    hour = 0;
    isMinuteSet = false;
    isHourSet = false;
    pinMode(ENCODER_CLK, INPUT_PULLUP);  //encoder input setup
    pinMode(ENCODER_DT, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW), increment_status, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);
}


void countdown() {
    currentMillis = millis();

    if (hour > 0 || minute > 0) {
        display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
        if (currentMillis - previousMillis >= 60000) {
            previousMillis = currentMillis;
            Serial.println("we");
            if (minute > 0) {
                minute--;
                display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
            } else {
                if (hour > 0) {
                    hour--;
                    minute = 59;
                    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
                }
            }
        }

        //TO BE SPECIFIED IF LEFT OR RIGHT
        int proximity = digitalRead(LEFTPROX);
        if (proximity == LOW) {
            mood = ANGRY;
        } else {
            mood = NORMAL;
        }

        currentMillis = millis();

    } else {
        previousMillis = 0;
        status = TIMER_FINISHED;
    }
}



/*
*
* EYES
*
*
*/

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

uint8_t ANGRY_EYE[] = {
    0b00000000,
    0b00100000,
    0b01110000,
    0b01111000,
    0b01100100,
    0b01100110,
    0b00111100,
    0b00000000
};


uint8_t SAD_EYE[] = {
    0b00000000,
    0b00000100,
    0b00001110,
    0b00011110,
    0b00100110,
    0b01100110,
    0b00111100,
    0b00000000
};


uint8_t FRONT_EYE[] = {
    0b00000000,
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111100,
    0b00000000
};

uint8_t HAPPY_EYE[] = {
    0b00000000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b01100110,
    0b11000011,
    0b10000001,
    0b00000000
};

uint8_t PISSED_EYE[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000
};

COROUTINE(blink) {
    COROUTINE_LOOP() {
        close_eyes();
        COROUTINE_DELAY(50);
        front_eyes(currentEye);
        COROUTINE_DELAY(1500);
    }
}


MATRIX7219 screen = MATRIX7219(EYE_DIN, EYE_CS, EYE_CLK, 2);

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


void assign_eye(uint8_t EYE[8]) {
    for (int i = 0; i < 8; i++) {
        currentEye[i] = EYE[i];
    }
}

void blink_happy() {
    close_eyes();
    delay(50);
    front_eyes(HAPPY_EYE);
    delay(1000);
}


void eye_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
}



void assign_mood() {
    switch (mood) {
        case HAPPY:
            assign_eye(HAPPY_EYE);
            break;
        case NORMAL:
            assign_eye(FRONT_EYE);
            break;
        case ANGRY:
            assign_eye(ANGRY_EYE);
            break;
        case SAD:
            assign_eye(SAD_EYE);
            break;
        case PISSED:
            assign_eye(PISSED_EYE);
            break;
    }
}

/*
    IDLE MANAGEMENT
*/

/** Sets the robot to the idle position */
void go_idle() {
    // TODO
}

/*
*
*
* SETUP LOOP
*
*
*/

void setup() {
    status = START;
    mood = NORMAL;
    init_power();
    eye_setup();
    clock_setup();
    pinMode(LEFTPROX, INPUT_PULLUP);
    pinMode(RIGHTPROX, INPUT_PULLUP);
    init_phone_slots();
    Serial.begin(9600);
}

void loop() {
    switch (status) {

        case START:
            increment_minutes();
            display.showNumberDecEx(minute, 0b01000000, true);
            break;

        case SET_MINUTES:
            if (isMinuteSet == 0 && isHourSet == 0) {
                reset_encoder();
                Serial.println("Minutes set");
                isMinuteSet = true;
                setMinute = minute;
                display.showNumberDecEx(setMinute, 0b01000000, true);
                blink_happy();
            }
            increment_hours();
            display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
            break;

        case SET_HOURS:
            if (isMinuteSet == 1 && isHourSet == 0) {
                Serial.println("Hours set");
                isHourSet = true;
                setHour = hour;
                display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
                blink_happy();
                isHourSet = true;
                status = PHONE_CHECK;
            }
            break;

        case PHONE_CHECK:
            mood = NORMAL;
            check_phone();
            if (phonePresent == true & isMinuteSet == 1 && isHourSet == 1) {
                Serial.println("Timer starts!");
                isMinuteSet = 0;
                isHourSet = 0;
                status = TIMER_GOING;
            }
            break;

        case TIMER_GOING:
            countdown();
            break;

        case TIMER_FINISHED:
            Serial.println("Time finished!");
            mood = NORMAL;
            isMinuteSet = false;
            isHourSet = false;
            minute = 0;
            setHour = 0;
            setMinute = 0;
            reset_encoder();
            display.showNumberDecEx(0x00, 0b01000000, true);
            status = START;
            break;
    }
    assign_mood();
    if (power_status == OFF) {
        go_idle();
        shutdown();
    }
    blink.runCoroutine();
}