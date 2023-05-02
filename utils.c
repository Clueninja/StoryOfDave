/*
 * File:   utils.c
 * Author: Sam Brown, Cole P, Vin
 *
 * Contains utility functions for main.c
 */

#include "utils.h"


void ir_setup(){  // Sets IR distance PORTA bits for input
    TRISAbits.RA2 = 1;
    TRISAbits.RA3 = 1;
}
void adc_setup(){ 
    ADCON1 = 0b00001101;
    ADCON2 = 0b10000010;
}
void sensor_setup(){
    // set distance sensors to input
    TRISAbits.RA0=1;
    TRISAbits.RA1=1;
}
unsigned int adc_value(enum Side side){
    ADCON0 = side<<2 | 0b11;
    while(ADCON0bits.GO);
    int value = (ADRESH & 0x03)<<8 | ADRESL;
    return value;
}

void motor_setup(){
    ADCON1 = 0xFF;
    PR2 = 0b11111111 ;     //set period of PWM,610Hz
    T2CON = 0b00000111 ;   //Timer 2(TMR2)on, prescaler = 16 
    CCP1CON = 0b00001100;   //enable CCP1 PWM
    CCP2CON = 0b00001100;   //enable CCP2 PWM
    TRISCbits.RC2=0;    //left pwm
    TRISCbits.RC1=0;    //right pwm
    // setup right motor
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    //setup left motor
    TRISAbits.RA4 = 0;
    TRISAbits.RA5 = 0;
    // setup motor encoders
    TRISCbits.RC0=1;
    TRISCbits.RC5=1;
    CCPR1L = 0;             //turn left motor off
    CCPR2L = 0;             //turn Right motor off
    
}

void wait(int del){     	 //delay function
	int c;
	for(c=0;c<del;c++)
    	__delay_ms(10);
	return;
} 


void motor(enum Side mot, enum Direction dir, unsigned int power){
    switch (mot){
    case Right:
        switch(dir){
            // Forwards works
        case Forwards:
            PORTBbits.RB0 = 0;
            PORTBbits.RB1 = 1;
            break;
        case Backwards:
            PORTBbits.RB0 = 1;
            PORTBbits.RB1 = 0;
            break;
            //Break works
        case Brake:
            PORTBbits.RB0 = 1;
            PORTBbits.RB1 = 1;
            break;
            // Coast works
        case Coast:
            PORTBbits.RB0 = 0;
            PORTBbits.RB1 = 0;
            break;
        }
        CCP1CON = (0x0c)|((power&0x03)<<4);//0x0c enables PWM,then insert the 2 LSB
        CCPR1L = (power>>2) & 0xFF; //of markspaceL into CCP1CON and the higher 8 bits into CCPR1L
        break;
    case Left:
        switch(dir){
        case Forwards:
            PORTAbits.RA4 = 0;
            PORTAbits.RA5 = 1;
            break;
        case Backwards:
            PORTAbits.RA4 = 1;
            PORTAbits.RA5 = 0;
            break;
        case Brake:
            PORTAbits.RA4 = 1;
            PORTAbits.RA5 = 1;
            break;
        case Coast:
            PORTAbits.RA4 = 0;
            PORTAbits.RA5 = 0;
            break;
        }
        CCP2CON = (0x0c)|((power&0x03)<<4);//0x0c enables PWM,then insert the 2 LSB
        CCPR2L = (power>>2) & 0xFF; //of power into CCP2CON and the higher 8 bits into CCPR2L
        break;
    }
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
void rotate(enum Side side, int degrees){
    motor(Right, Brake, 0);
    motor(Left, Brake, 0);
    wait(20);
    switch(side){
        case Right:
            motor(Left, Forwards, 400);
            motor(Right, Backwards, 400);
            break;
        case Left:
            motor(Right, Forwards, 400);
            motor(Left, Backwards, 400);
            break;
    }
    wait_degrees(degrees);
    motor(Right, Brake, 0);
    motor(Left, Brake, 0);
}