/*
 * File:   main.c
 * Author: Sam, Vin, Cole
 *
 * Created on 02 March 2023, 12:23
 */


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>



#pragma config OSC = HS	//High speed resonator
#pragma config WDT = OFF   //Watchdog timer off
#pragma config LVP = OFF   //Low voltage programming disabled
#pragma config PWRT = ON   //Power up timer on
#define _XTAL_FREQ 10000000 // define clock frequency for __delay_10ms()
#define LED1 LATBbits.LATB2 	//LED1
#define LED2 LATBbits.LATB3 	//LED2
#define LED3 LATBbits.LATB4 	//LED3
#define LED4 LATBbits.LATB5 	//LED4


#include "utils.h"

#define DSENSOR 400

int led_flash(void);

void wait(int del); 	//generates a delay in multiples of 10ms

void rotate(enum Side side);

char beacon(enum Side side){
    char value;
    switch (side){
        case Left:
            value = PORTAbits.RA2;
            break;
        case Right:
            value = PORTAbits.RA3;
            break;
    }
    return !value;
}

void setup_distance_sensors(){
    // set distance sensors to input
    TRISAbits.RA0=1;
    TRISAbits.RA1=1;
}


void align_beacon(){
    while(1){
        char left = beacon(Left);
        char right = beacon(Right);
        if (left && right){
            motor(Left, Brake,0);
            motor(Right, Brake,0);
            return;
        }
        if (left){
            motor(Right, Forwards, HALF_RIGHT);
            motor(Left, Coast, HALF_LEFT);
        }else if (right){
            motor(Right, Coast, HALF_RIGHT);
            motor(Left, Forwards, HALF_LEFT);
        }else{
            motor(Right, Backwards, HALF_RIGHT);
            motor(Left, Forwards, HALF_LEFT);
            //motor(Left, Brake,0);
            //motor(Right, Brake,0);
            //return;
        }
    }
}

unsigned int adc_value(enum Side side){
    ADCON0 = side<<2;
    ADCON0 = ADCON0 | (0b11);
    while(ADCON0bits.GO);
    int value = ADRESL | ADRESH<<8;
    return value;
}

void evasive_action(enum Side side){
    
    rotate(!side);
    wait(20);
    motor(Left, Forwards, HALF_LEFT);
    motor(Right, Forwards, HALF_RIGHT);
    wait(100);
    
    rotate(side);
    wait(20);
    
    motor(Left, Forwards, HALF_LEFT);
    motor(Right, Forwards, HALF_RIGHT);
    wait(40);
    
    motor(Left, Brake, 0);
    motor(Right, Brake, 0);
    
}
 
void project(){
    adc_setup();
    motor_setup();
    ir_setup();
    setup_distance_sensors();
    
    led_flash();
    while(1){
        if (adc_value(Left)>DSENSOR || adc_value(Right)>DSENSOR){
            if (adc_value(Left)>DSENSOR)
                evasive_action(Left);
            if (adc_value(Right)>DSENSOR)
                evasive_action(Right);
            // if the beacon is not detected on the left side
            // then the previous obstacle is the beacon
            if(!(beacon(Left)||beacon(Right))){
                //end
                break;
            }
        }
        align_beacon();
       
        motor(Left,Forwards, HALF_LEFT);
        motor(Right, Forwards, HALF_RIGHT);
    }
    
    led_flash();
    while(1);
}
int main(void)
{
    project();
}

void wait(int del){     	 //delay function
	int c;
	for(c=0;c<del;c++)
    	__delay_ms(10);
	return;
} 
void wait_degrees(int degrees){
    int count=0;
    while (count<degrees){
        if(PORTCbits.RC0){
            count++;
            while(PORTCbits.RC0);
        }
    }
}
void rotate(enum Side side){
    switch(side){
        case Right:
            motor(Left, Forwards, HALF_LEFT);
            motor(Right, Backwards, HALF_RIGHT);
            wait_degrees(180);
            motor(Right, Brake, 0);
            motor(Left, Brake, 0);
            break;
        case Left:
            motor(Right, Forwards, HALF_RIGHT);
            motor(Left, Backwards, HALF_LEFT);
            wait_degrees(90);
            motor(Right, Brake, 0);
            motor(Left, Brake, 0);
            break;
    }
}

int led_flash(void)
{
    TRISB=0b11000000; 	  	    //configure Port B, RB0 to RB5 as outputs
    LATB=0;               	 //turn all LEDs off
    for (int i = 0; i < 3; i++){
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        wait(100);
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
        wait(100);
	}
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
}


