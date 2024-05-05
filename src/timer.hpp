/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

#include <Arduino.h>
#include <TM1637Display.h>

/** Representation of the timer display. */
class Timer {
    public:
        /** Initializes the timer.
         *
         * @param displayCLK    the display clock pin
         * @param displayDIO    the display data i/o pin
        */
        Timer(uint8_t displayCLK, uint8_t displayDIO);

        /** Controls the status of the display.
         *
         * @param value the display status value
         */
        void setDisplay(bool value);

        /** Controls the blinking of the display.
         *
         * @param value the blink status setting
        */
        void setBlink(bool value);

        /** Changes the current time. The amount can be positive or negative,
         * so the time can be decreased by calling the same function.
         *
         * @param delta the amount of seconds to add/subtract
        */
        void increaseSeconds(int16_t delta);

        /** Controls the timer status.
         *
         * @param value the timer control setting,
        */
        void setTimerOn(bool value);

        /** Sets the time to zero. */
        void resetTimer();

    private:
        bool timerOn;
        bool blink;
        uint8_t minutes, seconds;
        TM1637Display display;
};