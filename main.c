/*
 * File:   main.c
 * Author: Sam, Vin, Cole
 *
 * Created on 02 March 2023, 12:23
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#pragma config OSC = HS    	//High speed resonator
#pragma config WDT = OFF  	 //Watchdog timer off
#pragma config LVP = OFF   	//Low voltage programming disabled
#pragma config PWRT = ON   	//Power up timer on

#include "I2C.h"

void main(void){
  unsigned char linesensor;    	 //Store raw data from sensor array
  TRISC = 0xFF;                	 //Set PORTC as inputs
  TRISB = 0x00;                 	//Set PORTB as outputs
  LATB = 0x00;                  	//Turn All LEDs off
  I2C_Initialise();             	//Initialise I2C Master 
    while(1)
  {
    I2C_Start();                	//Send Start condition to slave
    I2C_Write(0x7C);            	//Send 7 bit address + Write to slave
    I2C_Write(0x11);            	//Write data, select RegdataA and send to slave
    I2C_RepeatedStart();        	//Send repeat start condition
    I2C_Write(0x7D);            	//Send 7 bit address + Read
    linesensor=I2C_Read();      	//Read  the IR sensors 
    LATB=linesensor;            	//Output to LEDs
    I2C_Stop();                 	//Send Stop condition
   }
}




