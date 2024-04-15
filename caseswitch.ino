#include <MATRIX7219.h>
#include <AceRoutine.h>
#include <EncoderButton.h>

bool phonePresent;
enum Status {START, SET_MINUTES, SET_HOURS, TIMER_GOING, HAND_DETECTED, TIMER_FINISHED};

volatile Status status;

void setup(){
  status = START;
  init_leds();
  init_phone_slots();
  eyes_setup();

}

void loop(){
  switch(status){
    case START: 
      init_phone_slots();
      check_phone();
      break;
    case SET_MINUTES:
      break;
    case SET_HOURS:
      if(phonePresent) 
      //if at least one phone is inside, the timer starts
        status = TIMER_GOING;
      break;
    case TIMER_GOING:
      
      
      break;
    case HAND_DETECTED:
      break;
    case TIMER_FINISHED:
      break;
  }


}