#include <AceRoutine.h>
#include <Servo.h>

#define LEFT_ARM 6
#define RIGHT_ARM 7

// milliseconds that refer to the angles 0 and 180 (obtained through calibration) 
#define SERVO_MIN 700
#define SERVO_MAX 2380

using namespace ace_routine;

typedef struct {
    volatile uint16_t state;
    Servo servo;
} ServoState;

ServoState l_a, r_a;

// angles (in degrees) that represents the range of oscillation
uint8_t angle_min = 30;
uint8_t angle_max = 150;

// minimum and maximum delay to set for the oscillation
uint16_t delay_min = 15;
uint16_t delay_max = 40;

/* Sets a cosinusoidal movement, with peak speed (minimum delay) on the center
 * of the movement.
 */
int speed(int angle) {
    return (delay_max-delay_min) / 2.0 * (cos(2*PI*angle/180.0)+1) + delay_min;
}

COROUTINE(left_arm) {
    COROUTINE_LOOP() {
        for(; l_a.state <= angle_max; l_a.state++) {
            l_a.servo.write(l_a.state);
            COROUTINE_DELAY(speed(l_a.state));
        }
        for(; l_a.state > angle_min; l_a.state--) {
            l_a.servo.write(l_a.state);
            COROUTINE_DELAY(speed(l_a.state));
        }
    }
}

COROUTINE(right_arm) {
    COROUTINE_LOOP() {
        for(; r_a.state <= angle_max; r_a.state++) {
            r_a.servo.write(r_a.state);
            COROUTINE_DELAY(speed(r_a.state));
        }
        for(; r_a.state > angle_min; r_a.state--) {
            r_a.servo.write(r_a.state);
            COROUTINE_DELAY(speed(r_a.state));
        }
    }
}

void setup_arms() {
    pinMode(LEFT_ARM, OUTPUT);
    pinMode(RIGHT_ARM, OUTPUT);
    l_a.state = angle_min;
    r_a.state = angle_min + 10;
    l_a.servo.attach(LEFT_ARM, SERVO_MIN, SERVO_MAX);
    r_a.servo.attach(RIGHT_ARM, SERVO_MIN, SERVO_MAX);
}

void loop_arms() {
    left_arm.runCoroutine();
    right_arm.runCoroutine();
}
