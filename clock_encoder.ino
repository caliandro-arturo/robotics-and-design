
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
  currentMillis = millis();

  if (hour > 0 || minute > 0) {
    display.showNumberDecEx(100 * hour + minute, 0b01000000, true);
    if (currentMillis - previousMillis >= 60000) {
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
      mood = ANGRY;
      setLedRed(0);
    }else{
      mood = NORMAL;
    }

    currentMillis = millis();

  }else{ 
    previousMillis = 0;
    status = TIMER_FINISHED;
   
  }
}





