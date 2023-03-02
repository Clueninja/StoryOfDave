/* 
 * File:   motor.h
 * Author: samb
 *
 * Created on 02 March 2023, 15:14
 */

#ifndef MOTOR_H
#define	MOTOR_H


#include <xc.h>
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

void motor(enum Side mot, enum Direction dir, unsigned char power);
void setup_motor();




#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

