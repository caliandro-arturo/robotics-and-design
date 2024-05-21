#include "src/arms.hpp"
#include "src/torso.hpp"
#include "src/ears.hpp"
#include "src/head.hpp"
#include "src/pins.hpp"
#include "src/powerManager.hpp"
#include "src/servoCalibration.hpp"


#include <AceRoutine.h>
#include <DFRobotDFPlayerMini.h>
#include <MATRIX7219.h>
#include <TM1637Display.h>

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
bool phoneRemovedFinished = 0;

uint8_t currentEye[8];
uint8_t revCurrentEye[8];

enum Mood { NORMAL,
            HAPPY,
            SAD,
            LICKING,
            ANGRY,
            STUDY,
            SLEEP,
            DISAPPOINTED };

volatile Mood mood, prevMood;

enum Status { IDLE,
              START,
              FEEDBACK_STATE,
              SET_MINUTES,
              SET_HOURS,
              PHONE_CHECK,
              TIMER_GOING,
              RELEASE_PHONE,
              TIMER_FINISHED };

volatile Status status;

Status previousStatus;
unsigned long currentNoInteraction;
unsigned long previousNoInteraction;
/* The time when the last feedback reaction occurred. */
unsigned long feedback_start_time;
/* Duration of the feedback reaction. */
const unsigned long feedback_duration = 3000;

int stop = 0;
int triggerFlag = 0;
int countPhoneIn = 0;
int oldCountPhoneIn = 0;

/* Indicates the last proximity sensor that detected an hand, `0` if none */
uint8_t lastHandPresence = 0;
/* Inhibits the proximity sensors if false. */
bool sense_hands = true;
/* The time when the last angry reaction occurred. */
unsigned long angry_start_time;
/* The time since the last rotation did start. */
unsigned long angry_start_rotation;
/* The last time an hand has been detected within the angry reaction. */
unsigned long angry_last_hand_detection_time = 0;
/* Amount of time after which the robot surrends and let the user take the phone. */
const unsigned long angry_threshold = 4000;
/* Amount of time that must pass without any hand detection for the robot
 * to go back to a neutral from being angry.
 */
const unsigned long angry_cooldown = 1000;
/* Amount of time that must elapse before a new angry reaction can occurr. */
const unsigned long angry_relax = 1000;
/* Duration of a full torso rotation, plus some delay to ensure that the
 * rotation has been completed.
 */
const unsigned long angry_rotation_duration = 1000;
/* The time when the last disappointment mood occurred. */
unsigned long disappointment_start_time;
/* Duration of the disapointment reaction. */
const unsigned long disappointment_duration = 2000;

/*
    BODY PARTS
*/
Arms *arms;
Ears *ears;
Head *head;
Torso *torso;

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
    if (digitalRead(SLOT0) == HIGH || digitalRead(SLOT1) == HIGH || digitalRead(SLOT2) == HIGH || digitalRead(SLOT3) == HIGH) {
        countPhoneIn = digitalRead(SLOT0) + digitalRead(SLOT1) + digitalRead(SLOT2) + digitalRead(SLOT3);
        phonePresent = true;
    } else {
        countPhoneIn = 0;
        phonePresent = false;
    }
}

/*
*
*
* MP3
*
*/
DFRobotDFPlayerMini mp3;
int current_meow = 2;

enum SFX {
    PRR_SFX = 1,
    HAPPY_SFX = 2,
    SAD_SFX = 3,
    DISAPPOINTED_SFX = 4,
    ANGRY_SFX_1 = 5,
    ANGRY_SFX_2 = 6,
    LICK_SFX = 7
};

void init_mp3() {
    if (!mp3.begin(Serial3)) {
        Serial.println("error");
        return;
    }
    mp3.volume(1);
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


COROUTINE(blink_timer) {
    COROUTINE_BEGIN();
    display.clear();
    display.setBrightness(0, false);
    COROUTINE_DELAY(70);
    display.setBrightness(7, true);
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(70);
    display.clear();
    display.setBrightness(0, false);
    COROUTINE_DELAY(70);
    display.setBrightness(7, true);
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(70);
    display.clear();
    display.setBrightness(0, false);
    COROUTINE_DELAY(70);
    display.setBrightness(7, true);
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(70);
    COROUTINE_END();
}

COROUTINE(blink_hours) {
    COROUTINE_BEGIN();
    static uint8_t colon_display[] = { 0, 0b10000000 };
    //minutes on, hours off
    display.setSegments(colon_display, 2, 4);
    display.showNumberDec(setMinute, true, 2, 2);
    COROUTINE_DELAY(100);
    //minutes on, hours on
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    //minutes on, hours off
    display.setSegments(colon_display, 2, 4);
    display.showNumberDec(setMinute, true, 2, 2);
    COROUTINE_DELAY(100);
    //minutes on, hours on
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    //minutes on, hours off
    display.setSegments(colon_display, 2, 4);
    display.showNumberDec(setMinute, true, 2, 2);
    COROUTINE_DELAY(100);
    //minutes on, hours on
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    COROUTINE_END();
}

COROUTINE(blink_minutes) {
    COROUTINE_BEGIN();
    static uint8_t empty_display[] = { 0, 0 };
    //minutes off, hours on
    display.setSegments(empty_display, 2, 2);
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    //minutes on and hours on
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(100);
    //minutes off, hours on
    display.setSegments(empty_display, 2, 2);
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    //minutes on and hours on
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(100);
    //minutes off, hours on
    display.setSegments(empty_display, 2, 2);
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);
    COROUTINE_DELAY(100);
    //minutes on and hours on
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    COROUTINE_DELAY(100);
    COROUTINE_END();
}

COROUTINE(blink_dots) {
    COROUTINE_LOOP() {
        display.showNumberDec(100 * setHour + setMinute, true);
        COROUTINE_DELAY(500);
        display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
        COROUTINE_DELAY(500);
    }
}

COROUTINE(blink_dots_only) {
    COROUTINE_LOOP() {
        static uint8_t colon_display[] = { 0, 0b10000000, 0, 0 };
        static uint8_t empty_display[] = { 0, 0, 0, 0 };
        display.setSegments(empty_display, 4);
        COROUTINE_DELAY(500);
        display.setSegments(colon_display, 4);
        COROUTINE_DELAY(500);
    }
}

void reset_encoder() {
    encoderPos = 0;
}

void reset_timer() {
    setMinute = 0;
    setHour = 0;
    isMinuteSet = 0;
    isHourSet = 0;
    minute = 0;
    hour = 0;
    reset_encoder();
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
    if (status == SET_MINUTES) {
        previousStatus = SET_MINUTES;
        status = FEEDBACK_STATE;
    } else if (status == SET_HOURS) {
        previousStatus = SET_HOURS;
        status = FEEDBACK_STATE;
    }
}

void encoderISR() {
    int MSB = digitalRead(ENCODER_CLK);
    int LSB = digitalRead(ENCODER_DT);

    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderPos--;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderPos++;
    }
    if (!triggerFlag)
        previousTriggerMillis = millis();
    if (status == SET_MINUTES || status == SET_HOURS)
        previousNoInteraction = millis();
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
    pinMode(ENCODER_DT, INPUT);
    pinMode(ENCODER_SW, INPUT_PULLUP);
    PCICR |= bit(digitalPinToPCICRbit(ENCODER_CLK));
    *digitalPinToPCMSK(ENCODER_CLK) |= bit(digitalPinToPCMSKbit(ENCODER_CLK))
                                       | bit(digitalPinToPCMSKbit(ENCODER_DT))
                                       | bit(digitalPinToPCMSKbit(ENCODER_SW));
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
    if (sense_hands == false) {
        lastHandPresence = 0;
        return;
    }
    if (digitalRead(LEFTPROX) == LOW) {
        lastHandPresence = LEFTPROX;
    } else if (digitalRead(RIGHTPROX) == LOW) {
        lastHandPresence = RIGHTPROX;
    } else lastHandPresence = 0;
}



/*
*
* EYES
*
*
*/

MATRIX7219 screen = MATRIX7219(EYE_DIN, EYE_CS, EYE_CLK, 2);

uint8_t SLEEPY_EYE[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11110000,
    0b00100000,
    0b01000000,
    0b111110000
};


uint8_t SLEEPY1_EYE[] = {
    0b00000000,
    0b00000000,
    0b11111100,
    0b00001000,
    0b00010000,
    0b01100000,
    0b11111100,
    0b00000000
};


uint8_t SLEEPY2_EYE[] = {
    0b01111111,
    0b00000010,
    0b00000100,
    0b00011000,
    0b00100000,
    0b01111111,
    0b00000000,
    0b00000000
};
uint8_t ANGRY_EYE[] = {
    0b11000000,
    0b11110000,
    0b11111100,
    0b11000111,
    0b11000011,
    0b11000011,
    0b01100010,
    0b00111100
};


uint8_t SAD_EYE[] = {
    0b00000001,
    0b00000011,
    0b00001111,
    0b00111011,
    0b11100011,
    0b11000011,
    0b11000010,
    0b01111100
};


uint8_t NORMAL_EYE[] = {
    0b00111100,
    0b01110110,
    0b01100110,
    0b11100111,
    0b11100111,
    0b01100110,
    0b01100110,
    0b00111100,
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

uint8_t LICKING_EYE[] = {
    0b00000000,
    0b00000000,
    0b00111100,
    0b01111110,
    0b01111110,
    0b11000011,
    0b10000001,
    0b00000000
};

uint8_t DISAPPOINTED_EYE[] = {
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
        static int i = 1;
        for (i = 1; i <= 4; i++) {
            screen.setRow(i, 0, 1);
            screen.setRow(8 - i + 1, 0, 1);
            screen.setRow(i, 0, 2);
            screen.setRow(8 - i + 1, 0, 2);
            COROUTINE_DELAY(30);
        }
        COROUTINE_DELAY(50);
        for (i = 4; i >= 1; i--) {
            screen.setRow(i, currentEye[i - 1], 1);
            screen.setRow(8 - i + 1, currentEye[8 - i], 1);
            screen.setRow(i, revCurrentEye[i - 1], 2);
            screen.setRow(8 - i + 1, revCurrentEye[8 - i], 2);
            COROUTINE_DELAY(30);
        }
        COROUTINE_DELAY(1500);
    }
}

COROUTINE(blink_eyes_once) {
    COROUTINE_BEGIN();
    static int i = 1;
    for (i = 1; i <= 4; i++) {
        screen.setRow(i, 0, 1);
        screen.setRow(8 - i + 1, 0, 1);
        screen.setRow(i, 0, 2);
        screen.setRow(8 - i + 1, 0, 2);
        COROUTINE_DELAY(30);
    }
    COROUTINE_DELAY(50);
    for (i = 4; i >= 1; i--) {
        screen.setRow(i, currentEye[i - 1], 1);
        screen.setRow(8 - i + 1, currentEye[8 - i], 1);
        screen.setRow(i, revCurrentEye[i - 1], 2);
        screen.setRow(8 - i + 1, revCurrentEye[8 - i], 2);
        COROUTINE_DELAY(30);
    }
    COROUTINE_DELAY(1500);
    COROUTINE_END();
}

COROUTINE(zzz_eyes) {
    COROUTINE_LOOP() {
        side_eyes(SLEEPY_EYE);
        COROUTINE_DELAY(800);
        side_eyes(SLEEPY1_EYE);
        COROUTINE_DELAY(400);
        side_eyes(SLEEPY2_EYE);
        COROUTINE_DELAY(400);
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
        revCurrentEye[i] = reverse(EYE[i]);
    }
}

void eye_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
}

void trigger_user() {
    currentMillis = millis();
    if (currentMillis - previousTriggerMillis >= 7000) {
        if (triggerFlag != 1) {
            mood = DISAPPOINTED;
            triggerFlag = 1;
        } else {
            stop = 1;
            if (!isMinuteSet && !isHourSet)
                blink_minutes.runCoroutine();
            else
                blink_hours.runCoroutine();
        }
    } else {
        display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    }
    if (currentMillis - previousTriggerMillis >= 9000) {
        mood = NORMAL;
        blink_minutes.reset();
        blink_hours.reset();
        stop = 0;
        display.setBrightness(7, true);
        display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
        triggerFlag = 0;
        Serial.println(mood);
        previousTriggerMillis = currentMillis;
    }
}

/*
    FULL BODY MANAGEMENT
*/

/** Initializes the body parts. */
void init_body_parts() {
    arms = new Arms(LEFTARM, RIGHTARM, ARMS_MIN, ARMS_MAX);
    torso = new Torso(BODYPIN, BODY_MIN, BODY_MAX);
    ears = new Ears(LEFTEAR, RIGHTEAR, EARS_MIN, EARS_MAX);
    head = new Head(HEADPIN, PETSENSOR, HEAD_MIN, HEAD_MAX);
}

/** Sets the robot to the idle position */
void go_idle() {
    arms->stop();
    head->stop();
    arms->setPosition(LEFTARM, 0);
    arms->setPosition(RIGHTARM, 0);
    torso->setPositionImmediate(0);
    ears->setPosition(LEFTEAR, 0);
    ears->setPosition(RIGHTEAR, 0);
    head->setPosition(0);
}

/** Sets the robot to the happy mood. Non-blocking. */
void go_happy() {
    arms->shake(0, 70);
    ears->setPosition(LEFTEAR, 60);
    ears->setPosition(RIGHTEAR, 60);
    head->shake(-10, 10);
    torso->setPosition(0);
}

/** Interrupts the happy movements. */
void happy_stop() {
    arms->stop();
    head->stop();
}

/** Sets the robot sad. Non-blocking. */
void go_sad() {
    ears->setPosition(LEFTEAR, 60);
    ears->setPosition(RIGHTEAR, 90);
}

/** Sets the robot in the study phase, that is the normal state, but with
 * the difference that the torso rotates at a slower speed. Non-blocking.
*/
void go_study() {
    arms->stop();
    head->stop();
    arms->setPosition(LEFTARM, 0);
    arms->setPosition(RIGHTARM, 0);
    torso->setPosition(0);
    ears->setPosition(LEFTEAR, 0);
    ears->setPosition(RIGHTEAR, 0);
    head->setPosition(0);
}

/** Initializes the anger of the robot. It is an initial setup, to call
 * only once.
*/
void go_angry() {
    arms->stop();
    head->stop();
    arms->setPosition(LEFTARM, 60);
    arms->setPosition(RIGHTARM, 60);
    torso->setPosition(lastHandPresence == LEFTPROX ? -80 : 80);
    head->setPosition(lastHandPresence == LEFTPROX ? 80 : -80, false);
}

/** Sets the robot as disappointed. Like the other functions, it is non-blocking. */
void go_disappointed() {
    arms->stop();
    head->stop();
    arms->shake(0, 30);
    head->setPosition(0);
    head->shake(-20, 20);
    ears->setPosition(LEFTEAR, 60);
    ears->setPosition(RIGHTEAR, 60);
    torso->setPosition(0);
}

/** Updates the body. */
void update_body() {
    arms->runCoroutine();
    torso->runCoroutine();
    head->runCoroutine();
}

/** Moves randomly the head around. */
COROUTINE(rand_head) {
    COROUTINE_LOOP() {
        COROUTINE_DELAY_SECONDS(10);
        head->setPosition(random(-45, 45));
    }
}

/** Makes the cat lick a random paw.
 * TODO reduce frequency
*/
COROUTINE(rand_licking_paw) {
    COROUTINE_LOOP() {
        COROUTINE_DELAY_SECONDS(20);
        mood = LICKING;
        front_eyes(LICKING_EYE);
        static uint8_t random_arm;
        static uint8_t other_arm;
        random_arm = random(2) ? LEFTARM : RIGHTARM;
        other_arm = random_arm == LEFTARM ? RIGHTARM : LEFTARM;
        sense_hands = false;
        ears->setPosition(LEFTEAR, 45);
        ears->setPosition(RIGHTEAR, 45);
        arms->setPosition(random_arm, 100);
        arms->setPosition(other_arm, 45);
        head->setPosition(random_arm == LEFTARM ? 30 : -30);
        mp3.play(LICK_SFX);
        COROUTINE_DELAY(500);
        sense_hands = true;
        COROUTINE_DELAY(2500);
        sense_hands = false;
        ears->setPosition(LEFTEAR, 0);
        ears->setPosition(RIGHTEAR, 0);
        arms->setPosition(random_arm, 0);
        arms->setPosition(other_arm, 0);
        head->setPosition(0);
        close_eyes();
        mood = STUDY;
        COROUTINE_DELAY(500);
        sense_hands = true;
    }
}

/*
    MOOD HANDLER
*/

void assign_mood() {
    if (mood == prevMood)
        return;
    prevMood = mood;
    blink_eyes.reset();
    switch (mood) {
        case NORMAL:
            assign_eye(NORMAL_EYE);
            break;
        case STUDY:
            assign_eye(NORMAL_EYE);
            break;
        case ANGRY:
            assign_eye(ANGRY_EYE);
            mp3.play(ANGRY_SFX_1);
            break;
        case HAPPY:
            assign_eye(HAPPY_EYE);
            mp3.play(PRR_SFX);
            break;
        case SAD:
            assign_eye(SAD_EYE);
            mp3.play(SAD_SFX);
            break;
        case DISAPPOINTED:
            assign_eye(DISAPPOINTED_EYE);
            mp3.play(DISAPPOINTED_SFX);
            break;
    }
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
    status = IDLE;
    mood = SLEEP;
    prevMood = NORMAL;  // Just so it is different from mood
    assign_mood();
    clock_setup();
    eye_setup();
    init_mp3();
    encoderPos = 0;
    pinMode(LEFTPROX, INPUT_PULLUP);
    pinMode(RIGHTPROX, INPUT_PULLUP);
    init_phone_slots();
    init_body_parts();
    go_idle();
    Serial.begin(115200);
    Serial3.begin(9600);
    if (power_status == OFF) {
        shutdown();
    }
}

void reset_phone_check() {
    phonePresent = false;
}

void check_interaction() {
    currentNoInteraction = millis();
    if (currentNoInteraction - previousNoInteraction >= 60000) {
        phoneRemovedFinished = true;
        status = TIMER_FINISHED;
    }
}


void loop() {
    switch (status) {
        case IDLE:
            if (mood != SLEEP) {
                // This must be done before everything else, cannot wait
                // for the end of the switch to set the pose.
                go_idle();
                mood = SLEEP;
            }
            blink_dots_only.runCoroutine();
            check_phone();
            if (phonePresent == true) {
                display.setBrightness(7, true);
                reset_encoder();
                increment_minutes();
                display.showNumberDecEx(minute, 0b01000000, true);
                previousTriggerMillis = millis();
                previousNoInteraction = millis();
                status = SET_MINUTES;
            }
            break;

        case SET_MINUTES:
            if (mood != NORMAL && mood != DISAPPOINTED) {
                go_idle();
                mood = NORMAL;
            }
            increment_minutes();
            if (!stop) {
                display.showNumberDecEx(minute, 0b01000000, true);
            }
            check_phone();
            if (!phonePresent) {
                phoneRemovedFinished = true;
                status = TIMER_FINISHED;
            }
            //check_interaction();
            trigger_user();
            break;

        case FEEDBACK_STATE:
            if (mood != HAPPY && mood != SAD) {
                feedback_start_time = millis();
                blink_timer.reset();
                blink_eyes_once.reset();
                if ((previousStatus == TIMER_GOING) || (isMinuteSet == 1 && isHourSet == 0 && 100 * hour + setMinute <= 30) || (phoneRemovedFinished)) {
                    go_sad();
                    phoneRemovedFinished = false;
                    mood = SAD;
                } else {
                    mood = HAPPY;
                    go_happy();
                }
            }
            blink_timer.runCoroutine();
            blink_eyes_once.runCoroutine();

            // Until the interaction is not expired, do not touch the status.
            if (millis() - feedback_start_time <= feedback_duration)
                break;
            happy_stop();
            // Select the new state
            if (previousStatus == TIMER_FINISHED) {
                isMinuteSet = false;
                isHourSet = false;
                minute = 0;
                hour = 0;
                setHour = 0;
                setMinute = 0;
                reset_encoder();
                status = IDLE;
            } else if (previousStatus == TIMER_GOING) {
                status = TIMER_GOING;
            } else if (previousStatus == SET_MINUTES) {
                reset_encoder();
                Serial.println("Minutes set");
                isMinuteSet = true;
                setMinute = minute;
                display.showNumberDecEx(setMinute, 0b01000000, true);
                previousTriggerMillis = millis();
                previousNoInteraction = millis();
                status = SET_HOURS;
            } else if (previousStatus == SET_HOURS) {
                Serial.println("Hours set");
                setHour = hour;
                display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
                isHourSet = true;
                previousNoInteraction = millis();
                status = PHONE_CHECK;
            }
            break;

        case SET_HOURS:
            if (mood != NORMAL && mood != DISAPPOINTED) {
                go_idle();
                mood = NORMAL;
            }
            increment_hours();
            display.setBrightness(7, true);
            if (!stop) {
                display.showNumberDecEx(100 * hour + setMinute, 0b01000000, true);
            }
            check_phone();
            if (!phonePresent) {
                phoneRemovedFinished = true;
                status = TIMER_FINISHED;
            }
            //check_interaction();
            trigger_user();
            break;

        case PHONE_CHECK:
            if (mood != NORMAL) {
                go_idle();
                mood = NORMAL;
            }
            display.setBrightness(7, true);
            display.showNumberDecEx(100 * setHour + setMinute, 0b01000000, true);
            check_phone();
            oldCountPhoneIn = countPhoneIn;
            if (!phonePresent) {
                check_interaction();
            } else {
                Serial.println("Timer starts!");
                status = TIMER_GOING;
                previousMillis = millis();
            }
            break;

        case TIMER_GOING:
            if (mood != STUDY && mood != ANGRY && mood != LICKING) {
                go_study();
                mood = STUDY;
                rand_licking_paw.reset();
            }
            countdown();

            check_hand();
            if (lastHandPresence != 0 && mood == STUDY && millis() - angry_last_hand_detection_time >= angry_relax) {
                angry_start_rotation = millis();
                angry_start_time = millis();
                rand_licking_paw.reset();
                go_angry();
                mp3.stop();
                mood = ANGRY;
            }
            if (mood == ANGRY) {
                if (lastHandPresence != 0) {
                    angry_last_hand_detection_time = millis();
                    if (millis() - angry_start_time >= angry_threshold) {
                        // The attempt to rescue the phone lasted too long, giving up
                        status = RELEASE_PHONE;
                    } else if (!torso->isMoving()) {
                        if (millis() - angry_start_rotation >= angry_rotation_duration) {
                            // Torso reached the extreme, going to the other side
                            angry_start_rotation = millis();
                            int8_t new_position = torso->getPosition() > 0 ? -80 : 80;
                            torso->setPosition(new_position);
                            head->setPosition(-new_position, false);
                        }
                    }
                } else if (millis() - angry_last_hand_detection_time >= angry_cooldown) {
                    // No detection inside the cooldown interval, reset
                    angry_last_hand_detection_time = millis();
                    go_study();
                    mood = STUDY;
                }
            } else {
                rand_licking_paw.runCoroutine();
            }

            check_phone();
            if (!phonePresent) {
                phoneRemovedFinished = true;
                status = TIMER_FINISHED;
            } else if (phonePresent && oldCountPhoneIn > countPhoneIn) {
                oldCountPhoneIn = countPhoneIn;
                previousStatus = TIMER_GOING;
                status = FEEDBACK_STATE;
            } else if (phonePresent && oldCountPhoneIn < countPhoneIn) {
                oldCountPhoneIn = countPhoneIn;
            }
            break;

        case RELEASE_PHONE:
            if (mood != DISAPPOINTED) {
                disappointment_start_time = millis();
                go_disappointed();
                mood = DISAPPOINTED;
            }
            // Does not go forward until the animation is finished
            if (millis() - disappointment_start_time <= disappointment_duration)
                break;
            check_phone();
            if (phonePresent)
                status = TIMER_GOING;
            else {
                phoneRemovedFinished = true;
                arms->stop();
                arms->setPosition(LEFTARM, 0);
                arms->setPosition(RIGHTARM, 0);
                status = TIMER_FINISHED;
            }
            break;

        case TIMER_FINISHED:
            Serial.println("Time finished!");
            num_blinks = 0;
            hour = 0;
            minute = 0;
            display.showNumberDecEx(0x00, 0b01000000, true);
            previousStatus = TIMER_FINISHED;
            status = FEEDBACK_STATE;
            break;
    }
    assign_mood();
    if (power_status == OFF) {
        go_idle();
        delay(1000);
        shutdown();
        status = IDLE;
    }
    if (status == IDLE && mood != HAPPY) {
        zzz_eyes.runCoroutine();
    } else if (mood != HAPPY && mood != SAD && mood != LICKING) {
        blink_eyes.runCoroutine();
    }

    update_body();
}