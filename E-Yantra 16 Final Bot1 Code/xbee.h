
int flag[33];
int givenstr[33]; //= {13, 22, 29, 10, 4, 28, 28, 28, 32, 13, 28, 22, 26, 32, 16};
int pyserial = 0;
int len_str  ;
unsigned char data = '0'; //to store received data from UDR1
unsigned char cmd;
 


void start_zigbee();

unsigned char data2;
int count = 0;
char note_char[15] = {'C','G','D','G','A','C','F','B','A','G','E','F','F','E','C'};
char note_num[15] =  {'6','7','6','8','8','8','8','8','7','6','7','6','7','8','7'};
int mnp_num[15] =  {28,4,13,10,16,22,26,29,32,33,19,9,21,18,1};
void uart2_init(void)
{
 UCSR2B = 0x00; //disable while setting baud rate
 UCSR2A = 0x00;
 UCSR2C = 0x06;
 UBRR2L = 0x5F; //set baud rate lo
 UBRR2H = 0x00; //set baud rate hi
 UCSR2B = 0x98;
}



char a = '0';

SIGNAL(SIG_USART2_RECV) 		// ISR for receive complete interrupt
{
	data2 = UDR2;
	if(data2 != 'f'){ 				
		int i = 0;
		for(i = 0;i<15 ;i++){
			if(a == note_char[i] && data2 == note_num[i]){
				givenstr[count] = mnp_num[i];
				count++;
				break;
			}	
		}
		a = data2;
	}else if(data2 == 'f'){
		
		len_str = count;
		for(int i =0;i<count;i++){
			lcd_print(1,3,givenstr[i],2);
			_delay_ms(300);
		}
		pyserial = 1;
			
	}	
}


void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
// UBRR0L = 0x47; //11059200 Hz
 UBRR0L = 0x5F; // 14745600 Hzset baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}

void send_string(){
	int i;
	
	UDR0 = 'A';
	_delay_ms(400);	

	for(i = 0;i < len_str;i++){
		lcd_print(1,4,givenstr[i],2);
		UDR0 = givenstr[i] + '0';
		_delay_ms(600);
	}
	
	UDR0 = 'm';
	_delay_ms(500);	

	
	
}

void start_zigbee(){
	while(data != 'S'){
	lcd_home();
	}
	
	send_string();

	while(cmd != 'm'){
		lcd_home();
	}
}


SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 
	if(cmd == 'm'){
		int f = data - '0';
		flag[f] = 1;
		lcd_print(1,15,f,2); 

	}
	if(data == 'm'){
		cmd = 'm';
	}
}

