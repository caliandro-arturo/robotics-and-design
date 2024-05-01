
#include <AceRoutine.h>
#include <EncoderButton.h>
#include <TM1637Display.h>
#include <MATRIX7219.h>
#include "src/rgbled.hpp"
#include "src/head.hpp"
#include "src/arms.hpp"
#include "src/body.hpp"
#include "src/eyes.hpp"


#define PROXIMITY_IR 6

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

Eyes eyes;


TM1637Display display(TIMER_CLK, TIMER_DIO);


void setup(){
  status = START;
  eyes.eye_setup();
  clock_setup();
  Serial.begin(9600);
  //init_phone_slots();
}

void loop(){
  switch(status){
    case START: 
      //Serial.println(status);
      //init_phone_slots();
      //check_phone();
      increment_minutes();
      display.showNumberDecEx(minute, 0b01000000, true);
      break;
    case SET_MINUTES:
      
      if(isMinuteSet==0 && isHourSet==0){
        reset_encoder();
        Serial.println("Timer");
        isMinuteSet = true;
        setMinute = minute;
        display.showNumberDecEx(setMinute, 0b01000000, true);
        eyes.blink_happy();
        setLedWhite(0);
        blinkLed(0, 100, 100);
      }
      increment_hours();
      display.showNumberDecEx(100*hour + minute, 0b01000000, true);
      break;
    case SET_HOURS:
      if(isMinuteSet==1 && isHourSet==0){
        Serial.println("Minutes set");
        isHourSet = true;
        setHour = hour;
        display.showNumberDecEx(100*setHour + setMinute, 0b01000000, true);
        eyes.blink_happy();
        isHourSet = true;
        setLedWhite(0);
        blinkLed(0, 100, 100);
      }
      break;
    case PHONE_CHECK:
      mood = NORMAL;
      if(isMinuteSet==1 && isHourSet==1){
        Serial.println("Hours set");
        isMinuteSet = 0;
        isHourSet = 0;
        setLedGreen(0);
        blinkLed(0, 100, 100);
      } 
      //check_phone();
      //if(phonePresent == true){
        Serial.println("ok");
        status = TIMER_GOING;
      //}
      break;
    case TIMER_GOING:
      //Serial.println(status);
      //setLedRed(0);
      //blinkLed(0, 100, 100);
      //if(isMinuteSet==1 && isHourSet==1){
      //blink.runCoroutine();
      countdown();
      eyes.assignEye(FRONT_EYE);
      //}
      
      break;
    case HAND_DETECTED:
      setLedRed(0);
      blinkLed(0, 100, 100);
      //blink.runCoroutine();
      break;
    case TIMER_FINISHED:
      eyes.assignEye(FRONT_EYE);
      break;
  }

 // enum Mood {NORMAL, HAPPY, SAD, ANGRY, PISSED };
  switch(mood){
    case NORMAL:
      eyes.assignEye(FRONT_EYE);
    break;
    case HAPPY:
      eyes.assignEye(HAPPY_EYE);
    break;
    case SAD:
      eyes.assignEye(SAD_EYE);
    break;
    case ANGRY:
      eyes.assignEye(ANGRY_EYE);
    break;
    case PISSED:
    break;
  }
  eyes.runCoroutine();

}