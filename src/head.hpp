/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include <stdint.h>
#include "AceRoutine.h"
#include "Servo.h"

using namespace ace_routine;

/** Representation of the head. */
class Head : public Coroutine {
    public:
        /** Instantiates the head, setting its position to the center
         * activating the pet sensor.
         *
         * @param headPin           the head control pin
         * @param petSensorPin      the pet sensor pin
         * @param minMicroseconds   the time representing 0° (in microseconds)
         * @param maxMicroseconds   the time representing 180° (in microseconds)
        */
        Head(uint8_t headPin,
             uint8_t petSensorPin,
             uint16_t minMicroseconds,
             uint16_t maxMicroseconds);

        /** Moves the head to the selected position.
         *
         * @param pos       the position to reach (in degrees)
         * @param immediate if the position should be reached as soon as possible
        */
        void setPosition(int8_t pos, bool immediate = true);

        /** Shakes the head at the maximum speed.
         * The movement should be interrupted manually, unless the last
         * parameter is set to true. This is a workaround to use the same
         * coroutine for simple movements.
         *
         * @param from              the starting angle (in degrees)
         * @param to                the ending angle (in degrees)
         * @param oneMovementOnly   if the movement should only occurr once
        */
        void shake(int8_t from, int8_t to, bool oneMovementOnly = false);

        /** Stops any movement occurring to the head. */
        void stop();

        int runCoroutine() override;

        ~Head();
    private:
        Servo head;
        uint8_t position;
        uint8_t from, to;
        uint16_t minDelay, maxDelay;
        bool canMove;
};