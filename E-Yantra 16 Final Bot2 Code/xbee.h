int givenstr[33];
int flag[33];
int len_str;
unsigned char data = '0';
unsigned char cmd = '0';
//Function to initialize ports
//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
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

int count = 0;

SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 

	if(cmd == 'm'){
		
		int f = data - '0';
		flag[f] = 1;
		lcd_print(1,15,f,2);
		
	}else if(data == 'm'){
		
		len_str = count;
		cmd = 'm';
		UDR0 = 'm';

	}else if(cmd == 'A'){
		int t = data - '0';
		givenstr[count] = t;
		flag[count] = 0;
		count++;
	}else if(data == 'A'){
		cmd = 'A';
		count = 0;

	}
	//lcd_cursor(2,13);
	//lcd_wr_char(cmd);
		
}



