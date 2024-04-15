
//#define ENCODER_A A2
//#define ENCODER_B A3
#define BUTTON_PIN 18
#define DEBOUNCING_PERIOD 100


#define LED0_RED 11
#define LED0_BLUE 9
#define LED0_GREEN 10

volatile int counter;
int lastEnA = LOW;
unsigned long lastTime = 0UL;

unsigned long previousMillis = 0;
unsigned long interval = 1000;



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



void clock_setup() {
  // set display brightness (7 maximum brightness and 0 minimum)
  Serial.begin(9600);
  counter=0;
  display.setBrightness(7);
  display.clear();
  minute = 1;
  hour = 0;
  //setMinute = 100;
  //setHour = 100;
  isMinuteSet = false;
  isHourSet = false;
  //pinMode(ENCODER_A, INPUT_PULLUP);         //encoder input setup
  //pinMode(ENCODER_B, INPUT_PULLUP);*/
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), increment_status, RISING); 
  init_leds();

}

/*void setMinute(){
  //setMinute = minute;
  display.showNumberDec(minute, true);
}

void setHour(){
  //setHour = hour;
  display.showNumberDec(100*hour + minute, true);
}*/
/*
void countdown() {
  Serial.println("Timer started!");
  while (hour > 0 || minute > 0) {
    // Display current time
    display.showNumberDec(100 * hour + minute, true);
    delay(minute*10*1000); // Wait for one second

    // Decrement minute
    if (minute > 0) {
      minute--;
    } else {
      // Decrement hour and reset minute to 59 if it's 0
      if (hour > 0) {
        hour--;
        minute = 59;
      }
    }
  }
  Serial.println("Time finished!");
  setLedGreen(0);
  isMinuteSet=0; 
  isHourSet=0;
  minute = 1;
  status = START;
  display.showNumberDecEx(0x00, 0b01000000, true); // Display "Time's up!"
}*/




//WITH MILLIS
void countdown() {
  Serial.println("Timer started!");
  unsigned long currentMillis = millis();

  while (hour > 0 || minute > 0) {
    // Display current time
    display.showNumberDec(100 * hour + minute, true);

    // Check if it's time to update the display
    if (currentMillis - previousMillis >= (hour*interval*60*60+ interval*minute*60)) {
      previousMillis = currentMillis;

      // Decrement minute
      if (minute > 0) {
        minute--;
      } else {
        // Decrement hour and reset minute to 59 if it's 0
        if (hour > 0) {
          hour--;
          minute = 59;
        }
      }
    }
    currentMillis = millis(); // Update current time
  }

  // Timer finished
  Serial.println("Time finished!");
  setLedGreen(0);
  isMinuteSet = false;
  isHourSet = false;
  minute = 1;
  status = START;
  blinkLed(0, 100, 100);
  display.showNumberDecEx(0x00, 0b01000000, true); // Display "Time's up!"
}





unsigned long lastButtonPressTime = 0;






//void encoder_loop() {

    /*int enA = digitalRead(ENCODER_A);         //read state of encoder inputs
    int enB = digitalRead(ENCODER_B);

    unsigned long timestamp = millis();
    // if state different then prev one and current state is falling signals encoder movement
    // here we have also simple debouncing of encoder
    if (enA == LOW && lastEnA == HIGH && timestamp > lastChangeTimestamp + DEBOUNCING_PERIOD){
        if (enB == HIGH){
            Serial.print("CCW -> ");
            counter--;
        }
        else{
            Serial.print("CW  -> ");
            counter++;
        }
        lastChangeTimestamp = timestamp;
        Serial.print("Counter: ");
        Serial.println(counter);
    }
    lastEnA = enA;
    
    minute = counter % 60;
    display.showNumberDec(minute, true);
  */

  //minute = counter;
  //display.showNumberDec(minute, true);
/*  byte buttonState = digitalRead(SW);
  unsigned long currentTime = millis();
  if (buttonState != lastButtonState && currentTime - lastButtonPressTime > DEBOUNCING_PERIOD) {
    lastButtonPressTime = currentTime;
    if (buttonState == LOW) {
      Serial.println("The button is pressed");
      Serial.println(isMinuteSet);
      Serial.println(isHourSet);
      if(isMinuteSet==0 && isHourSet==0){
        display.showNumberDec(minute, true);
        isMinuteSet = true;
        setLedGreen(0);
        blinkLed(0, 100, 100);
      }else if(isMinuteSet==1 && isHourSet==0){
        display.showNumberDec(100*hour + minute, true);
        isHourSet = true;
        setLedRed(0);
        blinkLed(0, 100, 100);
      }

    }
  
    lastButtonState = buttonState;
  }
  if(isMinuteSet==1 && isHourSet==1){
      countdown();
  }

}
*/
