#include <Arduino.h>
#include <Servo.h>

/** Representation of the head. */
class Head {
    public:
        /** Instantiates the head, setting its position to the center
         * activating the pet sensor and the ears.
         * 
         * @param headPin       the head control pin
         * @param leftEarPin    the left ear control pin
         * @param rightEarPin   the right ear control pin
         * @param petSensorPin  the pet sensor pin
         * @param minHeadUs     minimum value for head servo (in micros) 
         * @param maxHeadUs     maximum value for head servo (in micros)
         * @param minEarUs      minimum value for ears servos (in micros)
         * @param maxEarUs      maximum value for ears servos (in micros)
        */
        Head(uint8_t headPin,
             uint8_t leftEarPin,
             uint8_t rightEarPin,
             uint8_t petSensorPin,
             uint16_t minHeadUs,
             uint16_t maxHeadUs,
             uint16_t minEarUs,
             uint16_t maxEarUs);

        /** Moves the head to the selected position.
         * 
         * @param pos   the position to reach (in degrees)
        */
        void setPosition(uint8_t pos);

        /** Shakes the head at the maximum speed.
         * The movement should be interrupted manually.
         * 
         * @param from  the starting angle (in degrees)
         * @param to    the ending angle (in degrees)
         * 
        */
        void shake(uint8_t from, uint8_t to);

        /** Stops any movement occurring to the head. */
        void stop();

    private:
        Servo head;
        Servo leftEar, rightEar;
        bool isMoving;
};