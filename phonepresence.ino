//line tracking sensor
#define SLOT0 41
/*#define SLOT1 1
#define SLOT2 1
#define SLOT3 1*/


typedef struct{
  int pin;
  bool phoneIn; 
  //RGB_Led rgbled;
}PhoneSlot;

PhoneSlot phoneSlot[1];


void init_phone_slots(){
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
void check_phone(){
  int count = 0;
  //for(int i= 0; i<1; i++){
    if(digitalRead(phoneSlot[0].pin) == LOW){ //detected
      phonePresent = true;
      phoneSlot[0].phoneIn = true;
      setLedRed(0);
      count++;

    }else{
      phonePresent = false;
      phoneSlot[0].phoneIn = false;
      setLedGreen(0);
    }  
 // }
 // if (count>0)
  //  phonePresent = true;
}