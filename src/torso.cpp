/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include "torso.hpp"
#include "servoCalibration.hpp"

Torso::Torso(uint8_t bodyPin, uint16_t minMicroseconds, uint16_t maxMicroseconds)
    : minMicroseconds(minMicroseconds),
      maxMicroseconds(maxMicroseconds),
      minDelay(BODY_MIN_DELAY),
      maxDelay(BODY_MAX_DELAY) {
    pinMode(bodyPin, OUTPUT);
    torso.attach(bodyPin, minMicroseconds, maxMicroseconds);
    uint16_t init = (maxMicroseconds + minMicroseconds) / 2;  // this is 0°
    position = init;
    start = init;
    destination = init;
    torso.writeMicroseconds(init);
}

void Torso::setPosition(int8_t pos) {
    uint16_t posUs = angleToUs(pos);
    start = position;
    destination = posUs;
}

void Torso::setPositionImmediate(int8_t pos) {
    position = angleToUs(pos);
    start = position;
    destination = position;
    torso.writeMicroseconds(position);
}

int Torso::runCoroutine() {
    COROUTINE_LOOP() {
        COROUTINE_AWAIT(position != destination);
        position += (destination > start) ? 1 : -1;
        torso.writeMicroseconds(position);
        COROUTINE_DELAY_MICROS(speed());
    }
}

bool Torso::isMoving() {
    return position != destination;
}

Torso::~Torso() {
    torso.detach();
}

uint16_t Torso::angleToUs(int8_t angle) {
    return map(angle, -90 + BODY_DELTA, 90 - BODY_DELTA, maxMicroseconds, minMicroseconds);
}

uint16_t Torso::speed() {
    if (destination == start)
        return maxDelay;
    double m = (double)(maxDelay - minDelay) / (int)(destination - start);
    double q = minDelay - m * start;
    return m * position + q;
}

int8_t Torso::getPosition() {
    return map(position, maxMicroseconds, minMicroseconds, -90 + BODY_DELTA, 90 - BODY_DELTA);
}