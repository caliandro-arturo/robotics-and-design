/**
 * This header contains the pin configuration of the robot.
*/
#ifndef PINS
#define PINS

#define LEFTARM
#define RIGHTARM
#define LEFTEAR
#define RIGHTEAR
#define HEADPIN
#define BODYPIN

#define PETSENSOR

#define SLOT0
#define SLOT1
#define SLOT2
#define SLOT3

#define LED0_RED
#define LED0_BLUE
#define LED0_GREEN
#define LED1_RED
#define LED1_BLUE
#define LED1_GREEN
#define LED2_RED
#define LED2_BLUE
#define LED2_GREEN
#define LED3_RED
#define LED3_BLUE
#define LED3_GREEN

// Plug the encoder pins to pins of the same I/O port
#define ENCODER_CLK 10
#define ENCODER_DT 11
#define ENCODER_SW 12
#define ENCODER_PINS PINB
#define ENCODER_PCMSK PCMSK0
#define ENCODER_VEC PCINT0_vect

#define DISPLAY_CLK 6
#define DISPLAY_DIO 7

#endif