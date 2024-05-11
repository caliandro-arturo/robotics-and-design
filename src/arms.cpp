/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include "arms.hpp"
#include "pins.hpp"
#include "servoCalibration.hpp"

template<typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template<typename T> void swap(T &a, T &b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

Arms::Arms(
    uint8_t leftArmPin,
    uint8_t rightArmPin,
    uint16_t minMicroseconds,
    uint16_t maxMicroseconds)
    : leftArmPin(leftArmPin),
      rightArmPin(rightArmPin),
      minMicroseconds(minMicroseconds),
      maxMicroseconds(maxMicroseconds),
      leftArmFrom(0),
      rightArmFrom(0),
      leftArmTo(0),
      rightArmTo(0),
      leftArmPos(0),
      rightArmPos(0),
      minDelay(ARMS_MIN_DELAY),
      maxDelay(ARMS_MAX_DELAY),
      canMove(false) {
    pinMode(leftArmPin, OUTPUT);
    pinMode(rightArmPin, OUTPUT);
    leftArm.attach(leftArmPin, minMicroseconds, maxMicroseconds);
    rightArm.attach(rightArmPin, minMicroseconds, maxMicroseconds);
    leftArm.writeMicroseconds(maxMicroseconds);
    rightArm.writeMicroseconds(minMicroseconds);
}

void Arms::shake(uint8_t from, uint8_t to) {
    stop();
    oscillate(from, to, ARMS_SHAKE_DELAY, ARMS_SHAKE_DELAY);
}

void Arms::oscillate(
    uint8_t from,
    uint8_t to,
    uint16_t minDelay,
    uint16_t maxDelay) {

    stop();
    uint16_t fromUs = angleToUs(from);
    uint16_t toUs = angleToUs(to);
    this->minDelay = minDelay;
    this->maxDelay = maxDelay;
    if (leftArmTo - leftArmFrom < 0) {
        swap(fromUs, toUs);
        swap(from, to);
    }
    leftArmFrom = fromUs;
    leftArmTo = toUs;
    rightArmFrom = fromUs;
    rightArmTo = toUs;
    leftArm.writeMicroseconds(fromUs);
    leftArmPos = fromUs;
    rightArm.writeMicroseconds(toUs);
    rightArmPos = fromUs;
    canMove = true;
}

void Arms::stop() {
    canMove = false;
}

void Arms::reset() {
    minDelay = ARMS_MIN_DELAY;
    maxDelay = ARMS_MAX_DELAY;
}

void Arms::setPosition(uint8_t armPin, uint8_t angle) {
    if (armPin == leftArmPin)
        leftArm.write(angle);
    else if (armPin == rightArmPin)
        rightArm.write(angle);
    else
        return;
}

int Arms::runCoroutine() {
    COROUTINE_LOOP() {
        if (canMove) {
            leftArmPos += (leftArmTo >= leftArmFrom) ? 1 : -1;
            rightArmPos += (rightArmTo >= rightArmFrom) ? 1 : -1;
            leftArm.writeMicroseconds(leftArmPos);
            rightArm.writeMicroseconds(rightArmPos);
            COROUTINE_DELAY_MICROS(speed(leftArmPos));
            if (leftArmPos == leftArmTo) {
                swap(leftArmFrom, leftArmTo);
                swap(rightArmFrom, rightArmTo);
            }
        }
    }
}

uint16_t Arms::speed(uint16_t angle) {
    if (maxDelay == minDelay)
        return maxDelay;
    static uint16_t min = min(leftArmFrom, leftArmTo);
    static uint16_t max = max(leftArmFrom, leftArmTo);
    return (maxDelay - minDelay) / 2.0
               * (cos(
                      2 * PI
                      * ((double)angle - min)
                      / (max - min))
                  + 1)
           + minDelay;
}

uint16_t Arms::angleToUs(uint8_t angle) {
    return map(angle, 0, 180, minMicroseconds, maxMicroseconds);
}

Arms::~Arms() {
    leftArm.detach();
    rightArm.detach();
}
