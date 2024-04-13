#include <Arduino.h>
#include <Servo.h>

/** Representation of the body, intended as the torso. */
class Body {
    public:
        /** Instantiates the body servo motor.
         *
         * @param bodyPin           the body control pin
         * @param minMicroseconds   the time representing 0° (in microseconds)
         * @param maxMicroseconds   the time representing 180° (in microseconds) 
        */
        Body(uint8_t bodyPin,
             uint16_t minMicroseconds,
             uint16_t maxMicroseconds);

        /** Moves the body setting its position.
         * The provided angle should be in the range [-90, 90].
         *
         * @param pos   the desired orientation of the body.
        */
        void setPosition(uint8_t pos);

    private:
};