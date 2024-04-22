#include "ears.hpp"

Ears::Ears(
    uint8_t leftEarPin,
    uint8_t rightEarPin,
    uint16_t minMicroseconds,
    uint16_t maxMicroseconds)
    : leftEarPin(leftEarPin), rightEarPin(rightEarPin) {
    leftEar.attach(leftEarPin, minMicroseconds, maxMicroseconds);
    rightEar.attach(rightEarPin, minMicroseconds, maxMicroseconds);
}

void Ears::move(uint8_t earPin, uint16_t pos) {
    if (earPin == leftEarPin) {
        leftEar.write(pos);
    } else if (earPin == rightEarPin) {
        rightEar.write(pos);
    }
}

Ears::~Ears() {
    leftEar.detach();
    rightEar.detach();
}
