#define EYE_DIN 40
#define EYE_CS 44
#define EYE_CLK 42


MATRIX7219 screen = MATRIX7219(EYE_DIN, EYE_CS, EYE_CLK, 2);

void print_matrix(uint8_t image[8], uint8_t display = 1) {
    for (int i = 1; i <= 8; i++) {
        screen.setRow(i, image[i - 1], display);
    }
}

uint8_t reverse(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

void close_eyes() {
    for (int i = 1; i <= 4; i++) {
        screen.setRow(i, 0, 1);
        screen.setRow(8 - i + 1, 0, 1);
        screen.setRow(i, 0, 2);
        screen.setRow(8 - i + 1, 0, 2);
        delay(30);
    }
}

void side_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, EYE[i - 1], 2);
        screen.setRow(8 - i + 1, EYE[8 - i], 2);
        delay(30);
    }
}

void front_eyes(uint8_t EYE[]) {
    for (int i = 4; i >= 1; i--) {
        screen.setRow(i, EYE[i - 1], 1);
        screen.setRow(8 - i + 1, EYE[8 - i], 1);
        screen.setRow(i, reverse(EYE[i - 1]), 2);
        screen.setRow(8 - i + 1, reverse(EYE[8 - i]), 2);
        delay(30);
    }
}

#define PROXIMITY_IR 2
void assignEye(uint8_t EYE[8]){
  for(int i = 0; i<8; i++){
    currentEye[i] = EYE[i];
  }
}
void proximityInterrupt() {
    int proximity = digitalRead(PROXIMITY_IR);
    if (proximity == LOW && status == TIMER_GOING) {
        mood = ANGRY;
        assignEye(ANGRY_EYE);
        blink.reset();
        Serial.println("0");
        Serial.println("1");
    } else if(proximity == HIGH && status == TIMER_GOING) {
        mood = NORMAL;
        Serial.println("1");
        assignEye(UP_RIGHT_EYE);
    }
}



void eye_setup() {
    screen.begin();
    screen.setBrightness(1);
    screen.clear();
    print_matrix(UP_RIGHT_EYE, 1);
    print_matrix(UP_RIGHT_EYE, 2);
    pinMode(PROXIMITY_IR, INPUT_PULLUP); 
    //attachInterrupt(digitalPinToInterrupt(PROXIMITY_IR), proximityInterrupt, RISING); 
    mood = NORMAL;
    assignEye(UP_RIGHT_EYE);
    Serial.begin(9600);
}




/*void eyes_loop() {
    if (mood == ANGRY) {
        close_eyes();
        delay(50);
        front_eyes(ANGRY_EYE);
        delay(1500);
    } else if (mood == NORMAL) {
        close_eyes();
        delay(50);
        side_eyes(UP_RIGHT_EYE);
        delay(1500);
    }
}*/


/*
void loop(){
  blink.runCoroutine();
}
*/