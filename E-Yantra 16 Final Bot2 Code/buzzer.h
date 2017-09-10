//Function to initialize Buzzer
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;        //Setting PORTC 3 as output
 PORTC = PORTC & 0xF7;      //Setting PORTC 3 logic low to turnoff buzzer
}

void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}

void buzz(void)
{
	buzzer_on();
   _delay_ms(500);        
   buzzer_off();
   _delay_ms(500);
}

void final_buzz(void)
{
	buzzer_on();
   _delay_ms(5500);      //final buzz 5.5 sec  
   buzzer_off();
}
