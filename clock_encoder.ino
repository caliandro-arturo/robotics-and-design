
volatile int counter;
int lastEnA = LOW;
unsigned long lastTime = 0UL;

unsigned long previousMillis = 0;
unsigned long interval = 1000;

volatile int lastEncoded = 0;
unsigned long currentMillis;

void reset_encoder(){
  encoderPos = 0;
}

void increment_hours(){
  hour =  (encoderPos % 30)/10 ;
}

void increment_minutes(){
  minute = (encoderPos % 300)/5;
}


void increment_status(){   
  unsigned long timeNow = millis();
  if (timeNow - lastTime > DEBOUNCING_PERIOD){
    if(status == START)
      status = SET_MINUTES;
    else if (status == SET_MINUTES){
      status = SET_HOURS;
      //setMinute = minute;
    }else if (status == SET_HOURS){
      status = PHONE_CHECK;
      //setHour = hour;
    }
    lastTime = timeNow;
  }
}

void encoderISR() {
  int MSB = digitalRead(ENCODER_A);
  int LSB = digitalRead(ENCODER_B);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderPos++;
  } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderPos--;
  }

  lastEncoded = encoded;
}


void clock_setup() {
  counter=0;
  display.setBrightness(7);
  display.clear();
  minute = 0;
  hour = 0;
  isMinuteSet = false;
  isHourSet = false;
  pinMode(ENCODER_A, INPUT_PULLUP);         //encoder input setup
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), increment_status, RISING); 
  attachInterrupt(digitalPinToInterrupt(ENCODER_A),encoderISR, CHANGE);
  init_leds();

}


void countdown() {
  //Serial.println("Timer started!");
  currentMillis = millis();

  if (hour > 0 || minute > 0) {
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    if (currentMillis - previousMillis >= 60000/*(hour*interval*60*60+ interval*minute*60)*/) {
      previousMillis = currentMillis;
      Serial.println("we");
      if (minute > 0) {
        minute--;
        display.showNumberDecEx(100*hour + minute, 0b01000000, true);
      } else {
        if (hour > 0) {
          hour--;
          minute = 59;
          display.showNumberDecEx(100*hour + minute, 0b01000000, true);
        }
      }
    }
    int proximity = digitalRead(PROXIMITY_IR);
    if(proximity ==  LOW){
      //status = HAND_DETECTED;
      mood = ANGRY;
      assignEye(ANGRY_EYE);
      setLedRed(0);
    }else{
      mood = NORMAL;
      assignEye(FRONT_EYE);
    }
    currentMillis = millis();
    blink.runCoroutine();
  }else{ 

    Serial.println("Time finished!");
    setLedGreen(0);
    assignEye(FRONT_EYE);
    isMinuteSet = false;
    isHourSet = false;
    minute = 0;
    setHour = 0;
    setMinute = 0;
    reset_encoder();
    status = START;
    blinkLed(0, 100, 100);
    display.showNumberDecEx(0x00, 0b01000000, true); 
  }
}





