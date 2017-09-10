#include<string.h>
#include<stdlib.h>

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
volatile unsigned int Degrees; //to accept angle in degrees for turning

int prev = 12,pres = 13,next;
char * CW_60[] = {  "29 11 12","2 3 26", "25 3 4", "6 7 28", "27 7 8", "10 11 30","14 15 32", "31 15 16", "18 19 34","33 19 20","22 23 36","35 23 24","37 25 26","25 26 38","39 29 30","29 30 40","41 33 34","33 34 42","36 37 43","43 37 25","25 37 36","26 38 44","44 38 27","27 38 26","28 39 45","45 39 29","29 39 28","30 40 46","46 40 31","31 40 30","32 41 47","47 41 33","33 41 32","34 42 48","48 42 35","35 42 34","44 43 37","38 44 43","46 45 39","40 46 45","48 47 41","42 48 47"};
char * ACW_60[] = {"34 19 18","36 37 25","42 34 33","44 38 26","38 26 25","25 37 43","34 33 41","30 40 31","8 7 27","30 29 39","33 41 47","39 45 46","40 30 29","31 40 46","45 39 28","27 38 44","48 42 34","41 47 48","47 48 42","37 43 44","43 37 36","47 41 32","36 23 22","26 38 27","30 11 10","20 19 33","34 42 35","26 3 2","45 46 40","12 11 29","24 23 35","43 44 38","4 3 25","26 25 37","16 15 31","28 39 29","35 42 48","32 15 14","46 40 30","32 41 33","29 39 45","28 7 6"};
char * CW_120[] = {"2 3 25","25 3 26","26 3 4","6 7 27","27 7 28","28 7 8","10 11 29","29 11 30","30 11 12","14 15 31","31 15 32","32 15 16","18 19 33","33 19 34","34 19 20","22 23 35","35 23 36","36 23 24","26 25 3","3 26 25","30 29 11","11 30 29","34 33 19","19 34 33","36 37 42","42 37 43","44 38 39","39 38 27","28 39 38","38 39 45","46 40 41","41 40 31","32 41 40","40 41 47","48 42 37","37 42 35"};
char * ACW_120[] = {"33 19 18","28 7 27","29 11 10","25 26 3","36 23 35","29 30 11","12 11 30","24 23 36","45 39 38","3 25 26","43 37 42","40 41 32","11 29 30","42 37 36","47 41 40","16 15 32","35 23 22","33 34 19","27 38 39","8 7 28","30 11 29","25 3 2","20 19 34","19 33 34","26 3 25","35 42 37","4 3 26","41 40 46","38 39 28","32 15 31","27 7 6","31 40 41","31 15 14","39 38 44","37 42 48","34 19 33"};
char * straight[] = {"3	25 37","3 26 38","7 27 38","7 28 39","11 29 39","11 30 40","15 31 40","15 32 41","19 33 41","19 34 42","23 35 42","23 36 37","25 37 42","26 38 39","29 39 38","30 40 41","33 41 40","34 42 37","37 25 3","38 26 3","38 27 7","39 28 7","39 29 11","40 30 11","40 31 15","41 32 15","41 33 19","42 34 19","42 35 23","37 36 23","42 37 25","39 38 26","38 39 29","41 40 30","40 41 33","37 42 34"};
//Function to configure ports to enable robot's motion
void motion_pin_config (void)
{
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0;
	DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
	PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}

//ISR for right position encoder
ISR(INT5_vect)
{
	ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
	ShaftCountLeft++;  //increment left shaft position count
}


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
	unsigned char PortARestore = 0;

	Direction &= 0x0F; 		// removing upper nibbel for the protection
	PortARestore = PORTA; 		// reading the PORTA original status
	PortARestore &= 0xF0; 		// making lower direction nibbel to 0
	PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
	PORTA = PortARestore; 		// executing the command
}

void forward (void) //both wheels forward
{
	motion_set(0x06);
}

void back (void) //both wheels backward
{
	motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
	motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
	motion_set(0x0A);
}

void soft_left (void) //Left wheel stationary, Right wheel forward
{
	motion_set(0x04);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
	motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
	motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
	motion_set(0x08);
}

void stop (void)
{
	motion_set(0x00);  //Both wheel stationary
}


//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;

	while (1)
	{
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	stop(); //Stop robot
}

void left_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	left(); //Turn left
	angle_rotate(Degrees);
}


void right_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	right(); //Turn right
	angle_rotate(Degrees);
}


void soft_left_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_left(); //Turn soft left
	Degrees=Degrees*2;
	angle_rotate(Degrees);
}

void soft_right_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_right();  //Turn soft right
	Degrees=Degrees*2;
	angle_rotate(Degrees);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

char str[8];


void concat(int x, int y,int z){
     
	 
	 sprintf(str,"%d %d %d",x,y,z);
	
}
int status = 0;
void move_to_next()
{
	if(Front_Sharp_Sensor > 155){
		forward();
		velocity(150,150);
		_delay_ms(700);
	}

	status = 0;
	if(prev == next){
		//rotate 180
		right_degrees(180);
		
	}else if(prev <= 24 && pres <= 24 && next <= 24){
		
	}else{
	
		concat(prev,pres,next);
		int i = 0;
		for(i = 0;i<42;i++){
			
			if(status == 1)
				break;

			if(strcmp(str,CW_60[i]) == 0){
				//rotate CW 60
				_delay_ms(500);
				right_degrees(60);
				status = 1;

			}else if(strcmp(str,ACW_60[i]) == 0){
				//rotate ACW 60
				_delay_ms(500);
				left_degrees(60);
				status = 1;
			}
			
		}
		for(i = 0;i<36;i++){
			if(status == 1)
				break;
			if(strcmp(str,CW_120[i]) == 0){
				//rotate CW 120
				_delay_ms(500);
				right_degrees(120);
				status = 1;
			}else if(strcmp(str,ACW_120[i]) == 0){
				//rotate ACW 120
				_delay_ms(500);
				left_degrees(120);
				status = 1;
			}
		}

		
	}
}
