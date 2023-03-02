

void lab_4(){
    setup_adc();
    setup_ir();
    setup_motor();
    TRISB=0b11000000; 	  	    //configure Port B, RB0 to RB5 as outputs
    while(1){
        LATB=0;
        if (beacon(Left) && beacon(Right)){
            LED1=1;
            motor(Left,Forwards, 127);
            motor(Right, Forwards, 127);
        }else if (beacon(Left)){
            LED2=1;
            motor(Right, Forwards, 127);
            motor(Left, Backwards, 127);
        }else if (beacon(Right)){
            LED3=1;
            motor(Right, Backwards, 127);
            motor(Left, Forwards, 127);
        }else{
            motor(Right, Brake, 0);
            motor(Left, Brake,0);
        }
        wait(1);
    }
}
void lab_3(){
    setup_motor();
    setup_adc();
    setup_distance_sensors();
    motor(Right, Forwards, HALF_RIGHT);
    motor(Left, Forwards, HALF_LEFT);
    while(1){
        
        if (distance(Left)>DSENSOR || distance(Right)>DSENSOR){
            if (distance(Left)>DSENSOR)
                evasive_action(Left);
            if (distance(Right)>DSENSOR)
                evasive_action(Right);
        }
        motor(Right, Forwards, HALF_RIGHT);
        motor(Left, Forwards, HALF_LEFT);
        
    }
}
void lab_3_toggle_led(){
    setup_adc();
    TRISB=0b11000000;
    LATB=0;
    
    while(1){
        unsigned int left = distance(Right);
        if (left>400){
            LED1 = !LED1;
            wait(100);
        }
    }
}

void lab_2(){
    setup_motor();
    while(1){
        motor(Left, Forwards, 127);
        motor(Right, Forwards, 127);
        wait(100);
        motor(Left, Brake, 0);
        motor(Right, Brake, 0);
        wait(100);
        motor(Left, Backwards, 127);
        motor(Right, Backwards,127);
        wait(100);
        motor(Left, Brake, 0);
        motor(Right, Brake, 0);
        wait(100);
    }
}