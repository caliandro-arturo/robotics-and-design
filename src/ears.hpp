/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include <stdint.h>
#include <Servo.h>

/** Representation of the ears. */
class Ears {
    public:
        /** Instantiates the two ears. It is assumed that the calibration
         * for both the ears is the same.
         *
         * @param leftEarPin        the left ear control pin
         * @param rightEarPin       the right ear control pin
         * @param minMicroseconds   the time representing 0° (in microseconds)
         * @param maxMicroseconds   the time representing 180° (in microseconds)
        */
        Ears(uint8_t leftEarPin,
             uint8_t rightEarPin,
             uint16_t minMicroseconds,
             uint16_t maxMicroseconds);

        /** Moves the specified ear to the specified positon.
         *
         * @param earPin    the ear to move
         * @param angle     the position that the ear must reach (in degrees)
        */
        void move(uint8_t earPin, uint16_t pos);

        ~Ears();
    private:
        Servo leftEar, rightEar;
        uint8_t leftEarPin, rightEarPin;
};