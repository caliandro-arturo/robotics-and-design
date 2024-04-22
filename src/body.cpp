#include "body.hpp"
#include "servoCalibration.hpp"

uint16_t minDelay = BODY_MIN_DELAY;
uint16_t maxDelay = BODY_MAX_DELAY;

Body::Body(uint8_t bodyPin, uint16_t minMicroseconds, uint16_t maxMicroseconds)
    : minMicroseconds(minMicroseconds), maxMicroseconds(maxMicroseconds) {
    pinMode(bodyPin, OUTPUT);
    body.attach(bodyPin, minMicroseconds, maxMicroseconds);
    uint16_t init = (maxMicroseconds + minMicroseconds) / 2;  // this is 0°
    position = init;
    start = init;
    destination = init;
    body.writeMicroseconds(init);
}

void Body::setPosition(int8_t pos) {
    uint16_t posUs = angleToUs(pos);
    destination = position;
    start = position;
    destination = posUs;
}

int Body::runCoroutine() {
    COROUTINE_LOOP() {
        COROUTINE_AWAIT(position != destination);
        position += (destination > start) ? 1 : -1;
        body.writeMicroseconds(position);
        COROUTINE_DELAY_MICROS(speed());
    }
}

bool Body::isMoving() {
    return position != destination;
}

Body::~Body() {
    body.detach();
}

uint16_t Body::angleToUs(int8_t angle) {
    return map(angle, -90 + BODY_DELTA, 90 - BODY_DELTA, minMicroseconds, maxMicroseconds);
}

uint16_t Body::speed() {
    if (destination == start)
        return maxDelay;
    double m = (double)(maxDelay - minDelay) / (int)(destination - start);
    double q = minDelay - m * start;
    return m * position + q;
}
