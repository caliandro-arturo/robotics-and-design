#include <Arduino.h>
#include <AceRoutine.h>
#include <TM1637Display.h>

using namespace ace_routine;

/** Representation of the timer display. */
class Timer : public Coroutine {
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

        /** Changes the current time. The direction can be positive or negative,
         * so that the time can be decreased by calling the same function.
         *
         * @param direction the
        */
        void increaseMinutes(int8_t direction);

        /** Controls the timer status.
         *
         * @param value the timer control setting,
        */
        void setTimerOn(bool value);

        /** Sets the time to zero. */
        void resetTimer();

        int runCoroutine() override;

        ~Timer();

    private:
        bool timerOn;
        bool blink;
        bool colon;
        uint8_t digits[4];
        int8_t hours, minutes;
        TM1637Display display;
};