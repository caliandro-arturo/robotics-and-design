/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include <stdint.h>
#include <AceRoutine.h>
#include <Servo.h>

using namespace ace_routine;

/** Representation of the torso. */
class Torso : public Coroutine {
    public:
        /** Instantiates the torso servo motor.
         *
         * @param bodyPin           the torso control pin
         * @param minMicroseconds   the time representing 0° (in microseconds)
         * @param maxMicroseconds   the time representing 180° (in microseconds)
        */
        Torso(uint8_t bodyPin,
              uint16_t minMicroseconds,
              uint16_t maxMicroseconds);

        /** Moves the torso setting its position.
         * The provided angle should be in the range [-90, 90].
         *
         * @param pos   the desired orientation of the torso
        */
        void setPosition(int8_t pos);

        /** Sets the torso to the specified position, without waiting.
         *
         * @param pos   the desired orientation of the torso
        */
        void setPositionImmediate(int8_t pos);

        int runCoroutine() override;

        /** Returns the movement state of the torso. */
        bool isMoving();

        /** Gets the torso position, in degrees. */
        int8_t getPosition();

        ~Torso();

    private:
        uint16_t speed();
        uint16_t angleToUs(int8_t angle);
        uint16_t position, start, destination;
        uint16_t minMicroseconds, maxMicroseconds;
        uint16_t minDelay, maxDelay;
        Servo torso;
};