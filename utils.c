
#include "utils.h"


void ir_setup(){
    TRISAbits.RA2 = 1;
    TRISAbits.RA3 = 1;
}
void adc_setup(){
    ADCON1 = 0b00001101;
    ADCON2 = 0b10000010;
}
void motor_setup(){
    ADCON1 = 0b00001101;
    // setup pwm shared registers
    PR2 = 0b11111111 ;    	//set period of PWM
    T2CON = 0b00000111 ;   //Timer 2(TMR2) on, Prescaler = 16
    TRISCbits.RC2=0;    //left pwm
    TRISCbits.RC1=0;    //right pwm
    
    // setup right motor
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    CCP1CON = (0x0c);        //0x0c enables PWM module CCP1

    //setup left motor
    TRISAbits.RA4 = 0;
    TRISAbits.RA5 = 0;
    CCP2CON = (0x0c);        //0x0c enables PWM module CCP2
    
    // setup motor encoders
    TRISCbits.RC0=1;
    TRISCbits.RC5=1;
    
    motor(Left, Brake,0);
    motor(Right, Brake,0);
}


void motor(enum Side mot, enum Direction dir, unsigned char power){
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
        CCPR1L = power;
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
        CCPR2L = power;
        break;
    }
}

