#include "src/arms.hpp"
#include "src/torso.hpp"
#include "src/ears.hpp"
#include "src/head.hpp"
#include "src/pins.hpp"
#include "src/powerManager.hpp"
#include "src/servoCalibration.hpp"
#include "src/timer.hpp"

#include <AceRoutine.h>
#include <DFRobotDFPlayerMini.h>
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

enum Status { IDLE,
              START,
              HAPPY_STATE,
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


void init_phone_slots() {
    pinMode(SLOT0, INPUT);
    pinMode(SLOT1, INPUT);
    pinMode(SLOT2, INPUT);
    pinMode(SLOT3, INPUT);
}

//may be the opposite
void check_phone() {
    if (digitalRead(SLOT0) == HIGH || digitalRead(SLOT1) == HIGH || digitalRead(SLOT2) == HIGH || digitalRead(SLOT3) == HIGH)  //detected
        phonePresent = true;
    else
        phonePresent = false;
}

/*
*
*
* MP3
*
*/
DFRobotDFPlayerMini mp3;
int current_meow = 2;

void init_mp3() {
    if (!mp3.begin(Serial3)) {
        Serial.println("error");
        while (true);
    }
    mp3.volume(20);
}


/*
*
*
* CLOCK ENCODER
*
*/


TM1637Display display(TIMER_CLK, TIMER_DIO);

int num_blinks;
volatile int counter;
int lastEnA = LOW;
unsigned long lastTime = 0UL;

unsigned long previousMillis = 0;
unsigned long previousTriggerMillis = 0;
unsigned long interval = 1000;

volatile int lastEncoded = 0;
unsigned long currentMillis;


void blink_timer() {
    for (int i = 0; i < 3; i++) {
        display.clear();
        display.setBrightness(0, false);
        delay(70);
        display.setBrightness(7, true);
        display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
        delay(70);
    }
}


/*COROUTINE(blink_timer) {
    COROUTINE_LOOP() {
        if (num_blinks < 4) {
            display.clear();
            display.setBrightness(0, false);
            COROUTINE_DELAY(70);
            display.setBrightness(7, true);
            display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
            COROUTINE_DELAY(70);
            num_blinks++;
        } else {
            if (isMinuteSet && !isHourSet) {
                blink_happy();
                num_blinks = 0;
                status = SET_MINUTES;
            } else if (isMinuteSet && isHourSet) {
                blink_happy();
                num_blinks = 0;
                status = PHONE_CHECK;
            }
        }
    }
}*/

COROUTINE(blink_dots) {
    COROUTINE_LOOP() {
        display.showNumberDec(100 * setHour + setMinute, true);
        COROUTINE_DELAY(500);
        display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
        COROUTINE_DELAY(500);
    }
}

void reset_encoder() {
    encoderPos = 0;
}

void increment_hours() {
    if (encoderPos >= 0) {
        hour = (encoderPos % 30) / 10;
    } else {
        encoderPos = 29;
    }
}

void increment_minutes() {
    if (encoderPos >= 0) {
        minute = (encoderPos % 300) / 5;
    } else {
        encoderPos = 299;
    }
}


void increment_status() {
    if (status == SET_MINUTES || status == SET_HOURS)
        status = HAPPY_STATE;
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

ISR(PCINT0_vect) {
    int buttonState = digitalRead(ENCODER_SW);
    if (buttonState == LOW) {
        // Encoder pushbutton interrupt
        increment_status();
        return;
    }
    // Encoder rotate interrupt
    encoderISR();
}


void clock_setup() {
    counter = 0;
    display.setBrightness(7, true);
    display.clear();
    minute = 0;
    hour = 0;
    isMinuteSet = false;
    isHourSet = false;
    display.showNumberDecEx(minute, 0b01000000, true);
    pinMode(ENCODER_CLK, INPUT);  //encoder input setup
    pinMode(ENCODER_DT, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);
    PCICR |= bit(digitalPinToPCICRbit(ENCODER_CLK));
    *digitalPinToPCMSK(ENCODER_CLK) |= bit(digitalPinToPCMSKbit(ENCODER_CLK))
                                       | bit(digitalPinToPCMSKbit(ENCODER_DT))
                                       | bit(digitalPinToPCMSKbit(ENCODER_SW));
}


void trigger_user() {
    currentMillis = millis();
    if (currentMillis - previousTriggerMillis >= 15000 && !isMinuteSet) {
        Serial.println("we");
        previousTriggerMillis = currentMillis;
    }
}




void countdown() {
    currentMillis = millis();

    if (setHour > 0 || setMinute > 0) {
        if (currentMillis - previousMillis >= 60000) {
            previousMillis = currentMillis;
            Serial.println("we");
            if (setMinute > 0) {
                setMinute--;
            } else {
                if (setHour > 0) {
                    setHour--;
                    setMinute = 59;
                }
            }
        }
        currentMillis = millis();
        blink_dots.runCoroutine();
    } else {
        status = TIMER_FINISHED;
    }
}

void check_hand() {
    //TO BE SPECIFIED IF LEFT OR RIGHT
    int proximity = digitalRead(LEFTPROX);
    if (proximity == LOW) {
        mood = ANGRY;
    } else {
        mood = NORMAL;
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

COROUTINE(blink_eyes) {
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

/*COROUTINE(blink_happy) {
    close_eyes();
    COROUTINE_DELAY(50);
    front_eyes(HAPPY_EYE);
    COROUTINE_DELAY(1000);
}*/

void blink_happy() {
    close_eyes();
    delay(50);
    front_eyes(HAPPY_EYE);
    delay(950);
}


void eye_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
}



void assign_mood() {
    switch (mood) {
        case NORMAL:
            assign_eye(FRONT_EYE);
            break;
        case ANGRY:
            assign_eye(ANGRY_EYE);
            //mp3.play(4);
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
    BODY PARTS
*/
Arms *arms;
Torso *torso;
Ears *ears;
Head *head;

void init_body_parts() {
    arms = new Arms(LEFTARM, RIGHTARM, ARMS_MIN, ARMS_MAX);
    torso = new Torso(BODYPIN, BODY_MIN, BODY_MAX);
    ears = new Ears(LEFTEAR, RIGHTEAR, EARS_MIN, EARS_MAX);
    head = new Head(HEADPIN, PETSENSOR, HEAD_MIN, HEAD_MAX);
    delay(3000);
}

/*
    IDLE MANAGEMENT
*/

/** Sets the robot to the idle position */
void go_idle() {
    arms->stop();
    arms->setPosition(LEFTARM, 0);
    arms->setPosition(RIGHTARM, 0);
    torso->setPositionImmediate(0);
    ears->move(LEFTEAR, 0);
    ears->move(RIGHTEAR, 0);
    head->setPosition(0);
    delay(3000);
}

/*
*
*
* SETUP LOOP
*
*
*/

void setup() {
    init_power();
    num_blinks = 10;
    eye_setup();
    status = IDLE;
    mood = ANGRY;
    assign_mood();
    clock_setup();
    encoderPos = 0;
    pinMode(LEFTPROX, INPUT_PULLUP);
    pinMode(RIGHTPROX, INPUT_PULLUP);
    init_phone_slots();
    init_body_parts();
    Serial.begin(115200);
    Serial3.begin(9600);
    init_mp3();
}

void reset_phone_check() {
    phonePresent = false;
}


void loop() {
    switch (status) {
        case IDLE:
            mood = PISSED;
            check_phone();
            if (encoderPos != 0 || phonePresent == true) {
                display.setBrightness(7, true);
                increment_minutes();
                display.showNumberDecEx(minute, 0b01000000, true);
                previousTriggerMillis = millis();
                status = SET_MINUTES;
            }
            break;

        case SET_MINUTES:
            mood = NORMAL;
            increment_minutes();
            display.showNumberDecEx(minute, 0b01000000, true);
            trigger_user();
            break;

        case HAPPY_STATE:
            blink_timer();
            blink_happy();
            if (isMinuteSet && isHourSet) {
                isMinuteSet = false;
                isHourSet = false;
                status = IDLE;
            } else if (!isMinuteSet && !isHourSet) {
                reset_encoder();
                Serial.println("Minutes set");
                isMinuteSet = true;
                setMinute = minute;
                display.showNumberDecEx(setMinute, 0b01000000, true);
                status = SET_HOURS;
            } else if (isMinuteSet == 1 && isHourSet == 0) {
                Serial.println("Hours set");
                setHour = hour;
                display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
                isHourSet = true;
                status = PHONE_CHECK;
            }
            break;

        case SET_HOURS:
            increment_hours();
            display.setBrightness(7, true);
            display.showNumberDecEx(100 * hour + setMinute, 0b01000000, true);
            break;

        case PHONE_CHECK:
            mood = NORMAL;
            display.setBrightness(7, true);
            display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
            check_phone();
            if (phonePresent == true & isMinuteSet == 1 && isHourSet == 1) {
                Serial.println("Timer starts!");
                status = TIMER_GOING;
                previousMillis = millis();
            }
            break;

        case TIMER_GOING:
            countdown();
            check_hand();
            break;

        case TIMER_FINISHED:
            Serial.println("Time finished!");
            num_blinks = 0;
            minute = 0;
            hour = 0;
            setHour = 0;
            setMinute = 0;
            reset_encoder();
            display.showNumberDecEx(0x00, 0b01000000, true);
            status = HAPPY_STATE;
            break;
    }
    assign_mood();
    if (power_status == OFF) {
        go_idle();
        noInterrupts();
        shutdown();
        interrupts();
    }
    blink_eyes.runCoroutine();
}
//missing cases:1) waited too much while setting the timer 2) forgot phone in, timer ended