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

int main(void)
{
    // setup various registers for the devices onboard
    adc_setup();
    motor_setup();
    ir_setup();
    sensor_setup();
    unsigned char linesensor;    	 //Store raw data from sensor array
    // not sure which bits actually need to be set
    TRISC = 0xFF;                	 //Set PORTC as inputs
    TRISB = 0x00;                 	//Set PORTB as outputs
    LATB = 0x00;                  	//Turn All LEDs off
    I2C_Initialise();             	//Initialise I2C Master 
    
    int velocity = 800;
    int max_velocity = 800;
    int step = 20;
    
    for(;;)
    {
        int raw_adc = adc_value(Left);
        // int distance = convert_to_distance(raw_adc)
        // increment or decrement velocity depending on adc reading
        
        linesensor = read_line_sensor();
        LATB = linesensor;
        // int degrees = convert_to_degrees(linesensor);
        // pwm_left = calc_pwm(Left, velocity, degrees);
        // pwm_right = calc_pwm(Right, velocity, degrees);
        // motor(Left, Forwards, pwm_left);
        // motor(Right, Forwards, pwm_right);
        
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


