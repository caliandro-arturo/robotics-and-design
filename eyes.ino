
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


void assign_eye(uint8_t EYE[8]){
  for(int i = 0; i<8; i++){
    currentEye[i] = EYE[i];
  }
}

void blink_happy(){
  close_eyes();
  delay(50);
  front_eyes(HAPPY_EYE);
  delay(1000);
}


void eye_setup() {
  screen.begin();
  screen.setBrightness(1);
  screen.clear();
}


