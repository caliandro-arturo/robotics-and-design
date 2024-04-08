#include <Arduino.h>
#include <Servo.h>

/** Representation of the arms.
 * 
 * Both arms are handled here, so the class exposes general methods to
 * move both of them contemporarily.
 * 
 * The method `setPosition` allows to control each arm individually, so
 * it's better to rely on it.
 */
class Arms {
    public:
        /** Instantiates the two arms. It is assumed that both the servos
         * are calibrated with the same values.
         *
         * TODO add distinguished values if needed.
         *
         * @param leftArmPin       the left arm control pin
         * @param rightArmPin      the right arm control pin
         * @param minMicroseconds  the time representing 0° (in microseconds)
         * @param maxMicroseconds  the time representing 180° (in microseconds)
         */
        Arms(uint8_t leftArmPin,
             uint8_t rightArmPin,
             uint16_t minMicroseconds,
             uint16_t maxMicroseconds);

        /** Covers the specified phone slot.
         * 
         * @param slot the phone slot to cover
         */
        void coverSlot(uint8_t slot);

        /** Shakes briefly. This can be used when the fastest speed is
         * desired.
         * 
         * @param from  the starting point (in degrees)
         * @param to    the ending point (in degrees)
         */
        void shake(uint8_t from, uint8_t to);

        /** Set a constant oscillation movement. It is used to set
         *  a continuous movement, to be halted manually.
         * 
         * @param from     the starting point (in degrees)
         * @param to       the ending point (in degrees)
         * @param minDelay the minimum delay that the arms will meet
         *                 (in milliseconds)
         */
        void oscillate(uint8_t from, uint8_t to, uint8_t minDelay);

        /** Interrupts any movement that the arms are performing. */
        void stop();

        /** Sets the specified arm at the given angle.
         * 
         * @param armPin   the arm to move
         * @param angle    the angle (in degrees)
         */
        void setPosition(uint8_t armPin, uint8_t angle);

        ~Arms();
    private:
        bool isMoving;
        Servo leftArm;
        Servo rightArm;
};