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
    int K = 60;
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

int raw_adc_to_cm(int raw_adc){
    if (raw_adc >130)
        return (671-raw_adc)/43;
    return (195-raw_adc)/5;
}

void lane_change(){
    motor(Left, Forwards, 400);
    motor(Right, Forwards, 400);
    while(read_line_sensor() & 0b00100000 != 0);
    motor(Left, Forwards, 300);
    motor(Right, Forwards, 500);
    wait(80);
}

void line_entered(int *lap_count){
    *lap_count += 1;
    // switch lane
    if(*lap_count == 2){
        motor(Left, Forwards, 500);
        motor(Right, Forwards, 200);
        wait(80);
        lane_change();
    }else if (*lap_count == 4){
        motor(Left, Forwards, 400);
        motor(Right, Forwards, 200);
        wait(80);
        lane_change();
     // wait 5s then turn anticlockwise -> clockwise
    }else if(*lap_count == 3){
        motor(Left, Brake, 0);
        motor(Right, Brake, 0);
        wait(500);
        rotate(Right, 180);
    }else if(*lap_count > 4){
        motor(Left, Brake, 0);
        motor(Right, Brake, 0);
        //Final LED flash sequence
        led_flash();
        //Infinite loop end of program
        while(1);
    }
}
void detect_line(int IR_register,int *currently_on_line,int *lap_count)
{
    if(IR_register == 0xFF){
        if(*currently_on_line == 0){
            line_entered(lap_count);
        }
        *currently_on_line = 1;
    }
    else{
        *currently_on_line = 0;
    }
}
// value is 4 bit
void set_leds(char value){
    int temp = LATB & 0b11000011;
    value = value & 0x0F;
    LATB = temp | (value<<2);
}

void display_number(int number){
    for (int i =0; i<sizeof(int) * 2; i++){
        set_leds(number>>(i*4));
        wait(100);
    }
}
int main(void)
{
    

    // setup various registers for the devices on board
    
    // not sure which bits actually need to be set so 
    // I'll overwrite them in the setup
    //Set PORTC as inputs
    TRISC = 0xFF;
    //Set PORTB as outputs
    TRISB = 0x00;
    //Turn All LEDs off
    LATB = 0x00;
    
    motor_setup();
    adc_setup();
    sensor_setup();
    //Initialise I2C Master 
    I2C_Initialise();
    /*
    // This code reads out the value from the adc, in binary, from the LEDs.
    // this is how I got the pain chart
    while(1){
        set_leds(0);
        wait(100);
        set_leds(0xF);
        wait(100);
        set_leds(0);
        wait(100);
        int raw_adc = adc_value(Left);
        display_number(raw_adc);
    }
    */
    int currently_on_line = 0;      
    int lap_count = 0;
    long long error_int = 0;
    int velocity;
    int Kp = 30;
    int KI = 1;
    led_flash();
    for(;;)
    {
        
        int raw_adc = adc_value(Left);
        int distance = raw_adc_to_cm(raw_adc);
        
        /*
         * Calculate velocity using PI control
         * PI is used in this case as the velocity 
         * needs to be greater than 0 when there is 0 error
         * 
         * integer multiplication/ division can be quite poor so 
         * bit shifting could be used if KI is a power of 2
         * 
         * if no robot exists then distance will be around 50 and so 
         * should speed up as the integral ramps up, to reduce the impact of this
         * we could clamp the speed to a set value until an object comes close
        */
        int error = distance-10;
        error_int = error_int + error;
        velocity =  Kp * error;//  + error_int>>5;
        // Saturate output between maximum and minimum
        if (velocity > 450)
            velocity = 450;
        if (velocity <63)
            velocity = 63;

        unsigned char linesensor = read_line_sensor();
        linesensor = ~linesensor;
        detect_line(linesensor, &currently_on_line, &lap_count);
        set_leds(lap_count);
        int degrees = convert_to_degrees(linesensor);
        // Calculate PWM for the left wheel
        int pwm_left = calc_pwm(Left, velocity, degrees);
        // Calculate PWM for the right wheel
        int pwm_right = calc_pwm(Right, velocity, degrees);
        // Set the PWM for the left wheel
        motor(Left, Forwards, pwm_left);
        // Set the PWM for the right wheel
        motor(Right, Forwards, pwm_right);
    }
}


int led_flash(void)
{
    //configure Port B, RB0 to RB5 as outputs
    TRISB=0b11000000;
    //turn all LEDs off
    LATB=0;
    for (int i = 0; i < 5; i++){
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        wait(50);
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
        wait(50);
	}
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
}

