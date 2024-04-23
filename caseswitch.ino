
#include <AceRoutine.h>
#include <EncoderButton.h>
#include <TM1637Display.h>
#include <MATRIX7219.h>

#define PROXIMITY_IR 6

using namespace ace_routine;

uint8_t currentEye[8];

enum Mood {NORMAL, HAPPY, SAD, ANGRY, PISSED };

volatile Mood mood;
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


#define TIMER_CLK 5   
#define TIMER_DIO 4  


int setMinute;
int setHour;
byte lastButtonState = HIGH;

bool isMinuteSet;
bool isHourSet;

int minute;
int hour;
volatile int encoderPos = 0;



TM1637Display display(TIMER_CLK, TIMER_DIO);

bool phonePresent;
enum Status {START, SET_MINUTES, SET_HOURS, PHONE_CHECK, TIMER_GOING, HAND_DETECTED, TIMER_FINISHED};

volatile Status status;

void setup(){
  status = START;
  eye_setup();
  clock_setup();
  //init_phone_slots();
}



COROUTINE(blink) {
  COROUTINE_LOOP() {
      close_eyes();
      COROUTINE_DELAY(50);
      front_eyes(currentEye);
      COROUTINE_DELAY(1500);
  }
}



void loop(){
  switch(status){
    case START: 
      //Serial.println(status);
      //init_phone_slots();
      //check_phone();
      increment_minutes();
      display.showNumberDec(minute, true);
      break;
    case SET_MINUTES:
      
      if(isMinuteSet==0 && isHourSet==0){
        reset_encoder();
        Serial.println("Timer");
        isMinuteSet = true;
        setMinute = minute;
        display.showNumberDec(setMinute, true);
        blink_happy();
        setLedWhite(0);
        blinkLed(0, 100, 100);
      }
      increment_hours();
      display.showNumberDec(100*hour + minute, true);
      break;
    case SET_HOURS:
      if(isMinuteSet==1 && isHourSet==0){
        Serial.println("Minutes set");
        isHourSet = true;
        setHour = hour;
        display.showNumberDec(100*setHour + setMinute, true);
        blink_happy();
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
        //if(phonePresent) //if at least one phone is inside, the timer starts
          status = TIMER_GOING;
      }
      break;
    case TIMER_GOING:
      //Serial.println(status);
      //setLedRed(0);
      //blinkLed(0, 100, 100);
      //if(isMinuteSet==1 && isHourSet==1){
      //blink.runCoroutine();
      countdown();
      assignEye(FRONT_EYE);
      //}
      
      break;
    case HAND_DETECTED:
      setLedRed(0);
      blinkLed(0, 100, 100);
      //blink.runCoroutine();
      break;
    case TIMER_FINISHED:
      assignEye(FRONT_EYE);
      break;
  }
  blink.runCoroutine();

}