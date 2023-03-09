/*
 * File:   main.c
 * Author: Sam, Vin, Cole
 *
 * Created on 02 March 2023, 12:23
 */


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>


#define LED1 LATBbits.LATB2 	//LED1
#define LED2 LATBbits.LATB3 	//LED2
#define LED3 LATBbits.LATB4 	//LED3
#define LED4 LATBbits.LATB5 	//LED4

#include "I2C.h"
#include "utils.h"

int led_flash(void);
// helper function for reading line sensor, once matured might move to utils
int read_line_sensor(){
    int linesensor;
    I2C_Start();                	//Send Start condition to slave
    I2C_Write(0x7C);            	//Send 7 bit address + Write to slave
    I2C_Write(0x11);            	//Write data, select RegdataA and send to slave
    I2C_RepeatedStart();        	//Send repeat start condition
    I2C_Write(0x7D);            	//Send 7 bit address + Read
    linesensor=I2C_Read();      	//Read  the IR sensors 
    I2C_Stop();                 	//Send Stop condition
    return linesensor;
}
// hard coded values to convert from the raw line sensor register
// to a value in degrees 
int convert_to_degrees(unsigned char IR_register){
    switch(IR_register){
        case 0x01:
            return 12;
        case 0x03:
            return 10;
        case 0x02:
            return 9;
        case 0x06:
            return 7;
        case 0x04:
            return 5;
        case 0x0C:
            return 3;
        case 0x08:
            return 2;
        case 0x18:
            return 0;
        case 0x10:
            return -2;
        case 0x30:
            return -3;
        case 0x20:
            return -5;
        case 0x60:
            return -7;
        case 0x40:
            return -9;
        case 0xC0:
            return -10;
        case 0x80:
            return -12;
    }
    return 0;
}

// Calculates new PWM values based on the side of Dave the motor is on
// and the current velocity value
int calc_pwm(enum Side side, int velocity, int degrees){
    int K = 20;
    int pwm;
    switch (side){
        case Left:
            pwm =  velocity + K * degrees;
            if (pwm>1023)
                pwm = 1023;
            break;
        case Right:
            pwm =  velocity - K * degrees;
            if (pwm<0)
                pwm = 0;
            break;
    }
    return pwm;
}

int millivolts(int raw_adc){
    return raw_adc * 5;
}
int centimeter(int millivolts){
    //return ((millivolts-3000)*(millivolts - 3000))/100000 + 3;
    return 43 - (27*millivolts)/2700;
}
int calc_velocity(int centimeter, int max_velocity){
    if (centimeter > 60)
        return max_velocity;
    if (centimeter < 10)
            return 0;
    return (max_velocity * (centimeter-10))/30;
}

void lane_change(){
    rotate(Right, 45);
    motor(Left, Forwards, 200);
    motor(Right, Forwards, 200);
    LATB = 0;
    LED1 = 1;
    while(read_line_sensor() & 0xC0 == 0){
        wait(100);
    }
    LED1 = 0;
}

void line_entered(int* lap_count){
    *lap_count += 1;
    if(*lap_count == 2 || *lap_count == 4){       // switch lanes
        lane_change();
    }else if(*lap_count == 3){                   // wait 5s then turn anticlockwise -> clockwise
        wait(500);
        rotate(Right, 180);
    }else if(*lap_count > 4){
        led_flash();                            //Final LED flash sequence
        while(1);                               //Infinite loop end of program
    }
    return;
}
int detect_line(int IR_register,int* currently_on_line,int* lap_count)
{
    if(IR_register == 0xFF){
        if(*currently_on_line == 0){
            line_entered(lap_count);
            *currently_on_line = 1;
        }
        else{
            return;
        }
    }
    else{
        *currently_on_line = 0;
    }
    return;

}
int main(void)
{
    // setup various registers for the devices on board
    
    // not sure which bits actually need to be set so 
    // I'll overwrite them in the setup
    TRISC = 0xFF;                	 //Set PORTC as inputs
    TRISB = 0x00;                 	//Set PORTB as outputs
    LATB = 0x00;                  	//Turn All LEDs off
    
    motor_setup();
    adc_setup();
    sensor_setup();
    I2C_Initialise();             	//Initialise I2C Master 
    
    int currently_on_line = 0;      
    int lap_count = 0;

    int velocity;
    int max_velocity = 400;
    //int step = 20;
    for(;;)
    {
        int raw_adc = adc_value(Left);
        // stops when hand is in front, 
        // eventually change to increase and decrease speed with distance
        // 
        int mvolts = millivolts(raw_adc);
        
        if (mvolts >3000)
            velocity = 0;
        else if (mvolts <200)
            velocity = max_velocity;
        else{
            velocity = ((mvolts-200)*max_velocity)/2800;
        }
        //velocity = calc_velocity(distance, max_velocity);
        unsigned char linesensor = read_line_sensor();
        int degrees = convert_to_degrees(linesensor);
        int pwm_left = calc_pwm(Left, velocity, degrees);   // Calculate PWM for the left wheel
        int pwm_right = calc_pwm(Right, velocity, degrees); // Calculate PWM for the right wheel
        motor(Left, Forwards, pwm_left);                    // Set the PWM for the left wheel
        motor(Right, Forwards, pwm_right);                  // Set the PWM for the right wheel
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

