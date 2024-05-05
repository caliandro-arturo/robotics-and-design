/**
 * Author: Arturo Caliandro <arturo.caliandro@mail.polimi.it>
*/

/** This header takes every needed step to turn on and off the robot.
 *
 * The robot, when plugged, will never turn "off". Instead, the Arduino
 * will go in power down mode after setting the robot to the idle position.
 *
 * To save further energy, the step-down converters are controlled via a
 * relay.
*/

/** Status of the robot */
enum power {
    OFF,
    ON
};

/** Initializes the power button and the relay pins. If the power button
 * is off, the robot will immediately go in power down mode.
*/
void init_power();

/** Checks the status of the power button. It is meant to be called via
 * interrupt.
*/
void power_check();

/** Turns off the robot. It handles the switch-on phase too. */
void shutdown();
