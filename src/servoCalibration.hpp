/**
 * This header contains the calibration values (in microseconds) of the
 * servo components.
 * *_MIN and *_MAX indicate respectively the minimum and maximum position
 * of the corresponding servo, while *_MIN_DELAY and *_MAX_DELAY respectively
 * the minimum and the maximum delay between two servo writes (two increments
 * or decrements).
*/

#ifndef ServoCalibration
#define ServoCalibration

#define ARMS_MIN 500
#define ARMS_MAX 2070
#define ARMS_MIN_DELAY 1200
#define ARMS_MAX_DELAY 4000
#define ARMS_SHAKE_DELAY 700

#define BODY_MIN 550
#define BODY_MAX 2260
#define BODY_MIN_DELAY 800
#define BODY_MAX_DELAY 2000
/** 
 * It can happen that the servo moves less than the theoretical 180 degrees.
 * It is possible to restrict the maximum distance from the central position
 * by specifying here the missing angles to reach 90.
 * 
 * The calibration should be done by finding the maximum (or minimum) servo
 * position (in microseconds) plugging the gear to the theoretical 90 (or -90)
 * degree position, finding the opposite extreme position (so the minimum
 * if the maximum was considered before) and measuring the angle (with a
 * goniometer), to set here.
 * The last step is to find the position for which the servo reaches the
 * supplementary angle (180 minus the found angle) and set it as the new
 * maximum position. By doing this, it will be possible to reach the central
 * position (the angle 0) as the medium point of the two extremes.
*/
#define BODY_DELTA 7

#define EARS_MIN 550
#define EARS_MAX 2375
/* No delay is expected for ears movement right now. */

#define HEAD_MIN 500
#define HEAD_MAX 2400
#define HEAD_MIN_DELAY 600
#define HEAD_MAX_DELAY 2000

#endif