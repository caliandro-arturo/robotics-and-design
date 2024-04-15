//line tracking sensor
/*#define SLOT0 1
#define SLOT1 1
#define SLOT2 1
#define SLOT3 1



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
  init_leds();

}

//may be the opposite
void check_phone(){
  int count = 0;
  for(int i= 0; i<1; i++){
    if(digitalRead(phoneSlot[i].pin) == 0){
      phoneSlot[i].phoneIn = false;
      setLedGreen(i);
    }else{
      phoneSlot[i].phoneIn = true;
      setLedRed(i);
      count++;
    }  
  }
  if (count>0)
    phonePresent = true;
  else
    phonePresent = false;
}*/