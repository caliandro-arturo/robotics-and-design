#include <MATRIX7219.h>
#include <AceRoutine.h>
#include <EncoderButton.h>
#include <TM1637Display.h>
#include <AceRoutine.h>
#include <MATRIX7219.h>
#define DIN 11
#define CS 12
#define CLK 13
#define PROXIMITY_IR 19

using namespace ace_routine;

uint8_t currentEye[8];

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


#define CLK 3   // clock pin
#define DIO 2   // data pin


int setMinute;
int setHour;
byte lastButtonState = HIGH;

bool isMinuteSet;
bool isHourSet;

int minute;
int hour;




TM1637Display display(CLK, DIO);

bool phonePresent;
enum Status {START, SET_MINUTES, SET_HOURS, PHONE_CHECK, TIMER_GOING, HAND_DETECTED, TIMER_FINISHED};

volatile Status status;

void setup(){
  status = START;
  //init_phone_slots();
  eye_setup();
  clock_setup();

}

void blink_happy(){
  close_eyes();
  delay(50);
  side_eyes(HAPPY_EYE);
  delay(1000);
}

COROUTINE(blink) {
  COROUTINE_LOOP() {
     //if (mood == ANGRY) {
        close_eyes();
        COROUTINE_DELAY(50);
        front_eyes(currentEye);
        COROUTINE_DELAY(1500);
    /*} else if (mood == NORMAL) {
        close_eyes();
        COROUTINE_DELAY(50);
        side_eyes(UP_RIGHT_EYE);
        COROUTINE_DELAY(1500);
    } else if (mood == HAPPY){
        close_eyes();
        COROUTINE_DELAY(50);
        side_eyes(HAPPY_EYE);
        COROUTINE_DELAY(1500);
        
    }*/
    COROUTINE_DELAY(50);
  }
}



void loop(){
  switch(status){
    case START: 
      //Serial.println(status);
      //init_phone_slots();
      //check_phone();
      break;
    case SET_MINUTES:
      
      if(isMinuteSet==0 && isHourSet==0){
        Serial.println("Timer");
        display.showNumberDec(minute, true);
        blink_happy();
        isMinuteSet = true;
        setLedWhite(0);
        blinkLed(0, 100, 100);
      }
      break;
    case SET_HOURS:
      if(isMinuteSet==1 && isHourSet==0){
        Serial.println("Minutes set");
        display.showNumberDec(100*hour + minute, true);
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
      assignEye(UP_RIGHT_EYE);
      //}
      
      break;
    case HAND_DETECTED:
      setLedRed(0);
      blinkLed(0, 100, 100);
      //blink.runCoroutine();
      break;
    case TIMER_FINISHED:
      break;
  }
  blink.runCoroutine();

}