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
    position = pos + 90;
    head.write(position);
}

void Head::shake(int8_t from, int8_t to) {
    stop();
    int8_t &nearest = (abs((int)(from - position - 90)) < abs((int)(to - position - 90))) ? from : to;
    // pin in between
    if (from <= position && position <= to || to <= position && position <= from) {
        if (this->from > this->to) {
            swap(from, to);
        }
    } else
        head.write(nearest);
    this->from = from + 90;
    this->to = to + 90;
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
