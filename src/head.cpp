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

bool oneMovementOnly;

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

void Head::setPosition(int8_t pos, bool immediate = true) {
    stop();
    to = pos + 90;
    if (immediate) {
        from = pos + 90;
        position = pos + 90;
        head.write(position);
    } else {
        oneMovementOnly = true;
        shake(position - 90, pos, true);
    }

}

void Head::shake(int8_t from, int8_t to, bool oneMovementOnly = false) {
    ::oneMovementOnly = oneMovementOnly;
    stop();
    from += 90;
    to += 90;
    if (from > to && !oneMovementOnly) {
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
        if (position == to) {
            if (oneMovementOnly == true) {
                canMove = false;
                oneMovementOnly = false;
            } else {
                swap(from, to);
            }
        }
        COROUTINE_DELAY_MICROS(minDelay * 10);  // One degree is almost 10 microseconds
    }
}
