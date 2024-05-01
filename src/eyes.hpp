#ifndef EYES_HPP
#define EYES_HPP

#include <Arduino.h>
#include <AceRoutine.h>

#define EYE_DIN 10
#define EYE_CS 9
#define EYE_CLK 8
#define PROXIMITY_IR 7
#define DEBOUNCING_PERIOD 1000
using namespace ace_routine;

class Eyes: public Coroutine {
public:
    Eyes();
    void blink_happy();
    void proximityInterrupt();
    void eye_setup();
    void print_matrix(uint8_t image[8], uint8_t display);
    uint8_t reverse(uint8_t b);
    void close_eyes();
    void side_eyes(uint8_t EYE[]);
    void front_eyes(uint8_t EYE[]);
    void assignEye(uint8_t EYE[8]);
    uint8_t currentEye[8];
    unsigned long lastTimeEye;
    enum Mood {
        NORMAL,
        ANGRY
    } mood;
    int runCoroutine() override;

};

#endif
