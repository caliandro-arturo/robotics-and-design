#include <MATRIX7219.h>
#include <AceRoutine.h>


enum Status {START, SET_MINUTES, SET_HOURS, TIMER_GOING, HAND_DETECTED, TIMER_FINISHED};

volatile Status status;

void setup(){
  status = START;
  init_leds();

}

void loop(){
  switch(status){
    case START: 

      break;
    case SET_MINUTES:
      break;
    case SET_HOURS:
      break;
    case TIMER_GOING:
      break;
    case HAND_DETECTED:
      break;
    case TIMER_FINISHED:
      break;
  }


}