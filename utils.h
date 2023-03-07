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

void sensor_setup();

void motor_setup();
void ir_setup();
void adc_setup();
unsigned int adc_value(enum Side);
void motor(enum Side mot, enum Direction dir, unsigned char power);
void rotate(enum Side side, int degrees);


void wait(int del); 	//generates a delay in multiples of 10ms




#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

