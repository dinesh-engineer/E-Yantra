#define F_CPU 14745600

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int b2[16] = {8,9,10,11,12,13,14,15,16,17,18,19,28,29,30,31};
int angle_180[5] = {22,10,28,26,29};


#include "lcd.h"
#include "findpath.h"
#include "white_line_sensors.h"
#include "motion.h"
#include "buzzer.h"
#include "xbee.h"
#include "servo.h"




int MNP[9]= { 1,25,5,39,40,13,17,42,21};

int dest = 0;


void strike();

//Function to Initialize PORTS
void port_init()
{
	buzzer_pin_config();
	lcd_port_config();
	adc_pin_config();
	servo_pin_config();
	motion_pin_config();
	left_encoder_pin_config(); //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config	
}

void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	timer1_init();
	timer5_init();
	uart0_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	sei();   //Enables the global interrupts
}



void assign_node_to_mnp(int n){
	
	final_path_len = 48;
	if(n < 25){
		dest = n;
		findPath(pres,dest);
	}
	else{
		dest = MNP[n - 25];
		findPath(pres,dest);
	}
	
	
}

int mnp_count = 0;
int node_count;

void next_mnp(){
	//lcd_print(2,2,mnp_count,2);
	//lcd_print(2,4,len_str,2);

	if(mnp_count < len_str){
		int j,temp = 0;
		for(j = 0;j<16;j++){
			//lcd_print(2,2,givenstr[mnp_count],2);
			//lcd_print(2,4,b2[j],2);
			//_dealy_ms(200);
			if(givenstr[mnp_count] == b2[j]){
				assign_node_to_mnp(givenstr[mnp_count]);
				//lcd_print(2,2,dest,2);
				//lcd_print(2,4,pres,2);
				if(dest == pres){
					strike();
					temp = 1;
					break;
				}else{
					
					node_count = 1;
					next = final_path[1];
					move_to_next();
					break;
				}	
			}
		}
		if(j == 16){
			mnp_count++;
			next_mnp();
		}
		if(temp == 1){
		    next_mnp();
		}

		 
	}else{
		final_buzz();
	}
}

void strike(){
	
	while(flag[mnp_count - 1] == 0 && mnp_count != 0){
		lcd_print(2,13,flag[mnp_count - 1],1);
		_delay_ms(200);
	}
	int i;
	for(i=0;i<5;i++){
		if(givenstr[mnp_count] == angle_180[i]){
			left_degrees(200);
			break;
		}
	}
	//buzz();	
	servo_rotate();
	if(i != 5)
		right_degrees(200);

	flag[mnp_count] = 1;
	UDR0 = mnp_count + '0';
	mnp_count++;
	
}
void fun(){
	stop();
	_delay_ms(1000);
	node_count++;
	prev = pres;
	pres = next;
	if(pres == dest){
		strike();
		next_mnp();

	}else{
		next = final_path[node_count];
		forward();
		velocity(150,150);
		move_to_next();
	}
}


//Main Function
int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	
	UDR0 = 'S';
	

	while(cmd != 'm'){
		lcd_home();
	}

	lcd_set_4bit();

    next_mnp();
	
	
	while(1)
	{	
		lcd_set_4bit();
		
		get_sensor_values();
		
		if(	Front_Sharp_Sensor > 155){
			left_degrees(180);
			graph[pres][next] = 0;
			int temp;
			temp = next;
			next = prev;
			prev = temp;
			mnp_count--;
			next_mnp();
		}

		lcd_print(2,14,dest,2);

		lcd_print(2,2,prev,2);
		lcd_print(2,6,pres,2);
		lcd_print(2,10,next,2);

				
			

		if((Left_white_line<=threshold) && (Center_white_line<=threshold1) && (Right_white_line<=threshold))
		{
			//W W W
			back();
			velocity(100,100);	
			//stop();
		}
		else if((Left_white_line<=threshold) && (Center_white_line<=threshold1) && (Right_white_line>threshold))
		{
			//W W B	-- RIGHT
			forward();
			velocity(150,100);	
		}
		else if((Left_white_line<=threshold) && (Center_white_line>threshold1) && (Right_white_line<=threshold))
		{
			//W B W	-- STRAIGHT
			forward();
			velocity(130,130);
							
		}
		else if((Left_white_line<=threshold) && (Center_white_line>threshold1) && (Right_white_line>threshold))
		{
			fun();
		}
		else if((Left_white_line>threshold) && (Center_white_line<=threshold1) && (Right_white_line<=threshold))
		{
			//B W W	-- LEFT
			forward();
			velocity(100,150);		
		}
		else if((Left_white_line>threshold) && (Center_white_line<=threshold1) && (Right_white_line>threshold))
		{
			//B W B		
			back();
			velocity(100,100);
		}
		else if((Left_white_line>threshold) && (Center_white_line>threshold1) && (Right_white_line<=threshold))
		{
			fun();	
		}
		else if((Left_white_line>threshold) && (Center_white_line>threshold1) && (Right_white_line>threshold))
		{
			//B B B   -- NODE
			fun();
		}

	}
}
