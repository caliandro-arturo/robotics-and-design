  
//RGB LED

#define LED0_RED 3
#define LED0_BLUE 5
#define LED0_GREEN 6

#define LED1_RED 3
#define LED1_BLUE 5
#define LED1_GREEN 6

#define LED2_RED 3
#define LED2_BLUE 5
#define LED2_GREEN 6

#define LED3_RED 3
#define LED3_BLUE 5
#define LED3_GREEN 6




typedef struct{
  int redpin;
  int greenpin;
  int bluepin; 
  int colorState;
} RGB_Led;

RGB_Led rgbled[4];


void setLedWhite(int num){
  analogWrite(rgbled[num].redpin, 255);
  analogWrite(rgbled[num].greenpin, 255);
  analogWrite(rgbled[num].bluepin, 255);
  rgbled[num].colorState = 3;
}

void setLedRed(int num){
  analogWrite(rgbled[num].redpin, 255);
  analogWrite(rgbled[num].greenpin, 0);
  analogWrite(rgbled[num].bluepin, 0);
  rgbled[num].colorState = 1; 
}

void setLedGreen(int num){
  analogWrite(rgbled[num].redpin, 0);
  analogWrite(rgbled[num].greenpin, 128);
  analogWrite(rgbled[num].bluepin, 0);
  rgbled[num].colorState = 2;
}

void setLedOff(int num){
  analogWrite(rgbled[num].redpin, 0);
  analogWrite(rgbled[num].greenpin, 0);
  analogWrite(rgbled[num].bluepin, 0);
  rgbled[num].colorState = 0;
}


void blinkLed(int num, int onDuration, int offDuration) {
  int colorState = rgbled[num].colorState;
  switch (colorState) {
    case 1: // Red color
      for (int i = 0; i < 5; i++) {
        setLedOff(num); 
        delay(offDuration);
        setLedRed(num); 
        delay(onDuration);
      }
      break;
    case 2: // Green color
      for (int i = 0; i < 5; i++) {
        setLedOff(num); 
        delay(offDuration);
        setLedGreen(num); 
        delay(onDuration);
      }
      break;
    case 3: // White color
      for (int i = 0; i < 5; i++) {
        setLedOff(num);
        delay(offDuration);
        setLedWhite(num); 
        delay(onDuration);
      }
      break;
  }
}


void init_leds(){
  rgbled[0].redpin = LED0_RED;
  rgbled[0].greenpin = LED0_GREEN;
  rgbled[0].bluepin = LED0_BLUE;
  
  rgbled[1].redpin = LED1_RED;
  rgbled[1].greenpin = LED1_GREEN;
  rgbled[1].bluepin = LED1_BLUE;

  rgbled[2].redpin = LED2_RED;
  rgbled[2].greenpin = LED2_GREEN;
  rgbled[2].bluepin = LED2_BLUE;

  rgbled[3].redpin = LED3_RED;
  rgbled[3].greenpin = LED3_GREEN;
  rgbled[3].bluepin = LED3_BLUE;

  for(int i= 0; i<4; i++){
    pinMode(rgbled[i].redpin, OUTPUT);
    pinMode(rgbled[i].greenpin, OUTPUT);
    pinMode(rgbled[i].bluepin, OUTPUT);
    rgbled[i].colorState = 0;
    setLedWhite(i);
  }
}



