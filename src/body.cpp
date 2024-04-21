#include "body.hpp"

/* 
 * It can happen that the servo moves less than the theoretical 180 degrees.
 * It is possible to restrict the maximum distance from the central position
 * by specifying here the missing angles to reach 90.
 * 
 * The calibration should be done by finding the maximum (or minimum) servo
 * position (in microseconds) plugging the gear to the theoretical 90 (or -90)
 * degree position, finding the opposite extreme position (so the minimum
 * if the maximum was considered before) and measuring the angle (with a
 * goniometer), to set here.
 * The last step is to find the position for which the servo reaches the
 * supplementary angle (180 minus the found angle)and set it as the new
 * maximum position. By doing this, it will be possible to reach the central
 * position (the angle 0) as the medium point of the two extremes.
*/
#define DELTA 7

uint16_t min_delay = 800;
uint16_t max_delay = 2000;


Body::Body(uint8_t bodyPin, uint16_t minMicroseconds, uint16_t maxMicroseconds)
    : minMicroseconds(minMicroseconds), maxMicroseconds(maxMicroseconds) {
    pinMode(bodyPin, OUTPUT);
    body.attach(bodyPin, minMicroseconds, maxMicroseconds);
    uint16_t init = (maxMicroseconds + minMicroseconds) / 2;
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
    return map(angle, -90 + DELTA, 90 - DELTA, minMicroseconds, maxMicroseconds);
}

uint16_t Body::speed() {
    if (destination == start)
        return max_delay;
    double m = (double)(max_delay - min_delay) / (int)(destination - start);
    double q = min_delay - m * start;
    return m * position + q;
}
