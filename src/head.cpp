/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include "head.hpp"
#include "servoCalibration.hpp"

template<typename T> void swap(T &a, T &b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

Head::Head(uint8_t headPin,
           uint8_t petSensorPin,
           uint16_t minMicroseconds,
           uint16_t maxMicroseconds)
    : position(90),
      from(90),
      to(90),
      minDelay(HEAD_MIN_DELAY),
      maxDelay(HEAD_MAX_DELAY),
      canMove(false) {
    pinMode(headPin, OUTPUT);
    pinMode(petSensorPin, INPUT);
    head.attach(headPin, minMicroseconds, maxMicroseconds);
    head.write(position);
}

void Head::setPosition(int8_t pos) {
    stop();
    from = pos + 90;
    to = pos + 90;
    position = pos + 90;
    head.write(position);
}

void Head::shake(int8_t from, int8_t to) {
    stop();
    from += 90;
    to += 90;
    if (from > to) {
        swap(from, to);
    }
    int8_t &nearest = (abs((int)(from - position)) < abs((int)(to - position))) ? from : to;
    // pin in between
    if (position < from || position > to) {
        position = nearest;
        head.write(nearest);
    }
    this->from = from;
    this->to = to;
    canMove = true;
}

void Head::stop() {
    canMove = false;
}

Head::~Head() {
    stop();
    head.detach();
}

int Head::runCoroutine() {
    COROUTINE_LOOP() {
        COROUTINE_AWAIT(canMove);
        position += (from < to) ? 1 : -1;
        head.write(position);
        if (position == to)
            swap(from, to);
        COROUTINE_DELAY_MICROS(minDelay * 10);  // One degree is almost 10 microseconds
    }
}
