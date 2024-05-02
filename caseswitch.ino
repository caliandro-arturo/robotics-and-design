
#include <AceRoutine.h>
#include <EncoderButton.h>
#include <TM1637Display.h>
#include <MATRIX7219.h>


#define TIMER_CLK 5   
#define TIMER_DIO 4  

#define ENCODER_SW 18
#define ENCODER_A 19
#define ENCODER_B 20
#define DEBOUNCING_PERIOD 100

#define LED0_RED 11
#define LED0_BLUE 9
#define LED0_GREEN 10

#define EYE_DIN 40
#define EYE_CS 44
#define EYE_CLK 42

#define PROXIMITY_IR 6

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

using namespace ace_routine;

uint8_t currentEye[8];

enum Mood {NORMAL, HAPPY, SAD, ANGRY, PISSED };

volatile Mood mood;

enum Status {START, SET_MINUTES, SET_HOURS, PHONE_CHECK, TIMER_GOING, HAND_DETECTED, TIMER_FINISHED};

volatile Status status;


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


uint8_t  SAD_EYE[] = {
    0b00000000,
    0b00000100,
    0b00001110,
    0b00011110,
    0b00100110,
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



TM1637Display display(TIMER_CLK, TIMER_DIO);

COROUTINE(blink) {
  COROUTINE_LOOP() {
      close_eyes();
      COROUTINE_DELAY(50);
      front_eyes(currentEye);
      COROUTINE_DELAY(1500);
  }
}

void assign_mood(){
  switch(mood){
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

void setup(){
  status = START;
  mood = NORMAL;
  eye_setup();
  clock_setup();
  pinMode(PROXIMITY_IR, INPUT_PULLUP); 
  init_phone_slots();
  Serial.begin(9600);
}

void loop(){
  switch(status){

    case START: 
      increment_minutes();
      display.showNumberDecEx(minute, 0b01000000, true);
      break;

    case SET_MINUTES:
      if(isMinuteSet==0 && isHourSet==0){
        reset_encoder();
        Serial.println("Minutes set");
        isMinuteSet = true;
        setMinute = minute;
        display.showNumberDecEx(setMinute, 0b01000000, true);
        blink_happy();
        setLedWhite(0);
        blinkLed(0, 100, 100);
      }
      increment_hours();
      display.showNumberDecEx(100*hour + minute, 0b01000000, true);
      break;

    case SET_HOURS:
      if(isMinuteSet==1 && isHourSet==0){
        Serial.println("Hours set");
        isHourSet = true;
        setHour = hour;
        display.showNumberDecEx(100*setHour + setMinute, 0b01000000, true);
        blink_happy();
        isHourSet = true;
        setLedWhite(0);
        blinkLed(0, 100, 100);
        status = PHONE_CHECK;
      }
      break;

    case PHONE_CHECK:
      mood = NORMAL;
      check_phone();
      if(phonePresent == true & isMinuteSet==1 && isHourSet==1){
        Serial.println("Timer starts!");
        isMinuteSet = 0;
        isHourSet = 0;
        setLedGreen(0);
        blinkLed(0, 100, 100);
        status = TIMER_GOING;
      }
      break;

    case TIMER_GOING:
      countdown();
      break;
  
    case TIMER_FINISHED:
      Serial.println("Time finished!");
      setLedGreen(0);
      mood = NORMAL;
      isMinuteSet = false;
      isHourSet = false;
      minute = 0;
      setHour = 0;
      setMinute = 0;
      reset_encoder();
      blinkLed(0, 100, 100);
      display.showNumberDecEx(0x00, 0b01000000, true);
      status = START;
      break;
  }
  assign_mood();
  blink.runCoroutine();

}