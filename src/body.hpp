#include <Arduino.h>
#include <AceRoutine.h>
#include <Servo.h>

using namespace ace_routine;

/** Representation of the body, intended as the torso. */
class Body : public Coroutine {
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
        void setPosition(int8_t pos);

        int runCoroutine() override;

        /** Returns the movement state of the body. */
        bool isMoving();

    private:
        uint16_t speed();
        uint16_t angleToUs(int8_t angle);
        uint16_t position, start, destination;
        uint16_t minMicroseconds, maxMicroseconds;
        Servo body;
};