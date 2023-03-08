/* 
 * File:   motor.h
 * Author: samb
 *
 * Created on 02 March 2023, 15:14
 */

#ifndef UTILS_H
#define	UTILS_H


#include <xc.h>



#pragma config OSC = HS	//High speed resonator
#pragma config WDT = OFF   //Watchdog timer off
#pragma config LVP = OFF   //Low voltage programming disabled
#pragma config PWRT = ON   //Power up timer on
#define _XTAL_FREQ 10000000 // define clock frequency for __delay_10ms()


#define QUART_LEFT 64
#define QUART_RIGHT 61

#define HALF_LEFT 127
#define HALF_RIGHT 122
#define FULL_LEFT 255
#define FULL_RIGHT 247


#ifdef	__cplusplus
extern "C" {
#endif
enum Direction{
    Forwards,
    Backwards,
    Brake,
    Coast
};
enum Side{
    Left,
    Right
};
// sets the correct bits in TRISA for reading the distance sensor
void sensor_setup();

// sets the bits in TRISC and TRISB to configure the PWM for both motors
void motor_setup();

// sets the bits in TRISA for the IR sensor for detecting the beacon
void ir_setup();

// sets the bits in ADCONX for starting the ADC
void adc_setup();

// reads the value for the distance sensors on either side
unsigned int adc_value(enum Side);

// sets the pwm duty power, the direction for a particular motor
void motor(enum Side mot, enum Direction dir, unsigned int power);

// rotates the robot by degrees in a side
void rotate(enum Side side, int degrees);

// waits in multiples of 10ms, 1s is wait(100)
void wait(int del); 	//generates a delay in multiples of 10ms




#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

