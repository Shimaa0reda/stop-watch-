#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char count_sec1=0;
unsigned char count_sec2=0;
unsigned char count_min1=0;
unsigned char count_min2=0;
unsigned char count_hour1=0;
unsigned char count_hour2=0;
unsigned char i=0;
void timer()
{
	TCNT1 = 0;
	OCR1A = 1000;
	TIMSK |= (1<<OCIE1A);
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10)|(1<<CS12);
}
ISR(TIMER1_COMPA_vect)
{
	count_sec1++;
	if(count_sec1==10)
	{
		count_sec2++;
		count_sec1=0;
	}
	if(count_sec2==6)
	{
		count_min1++;
		count_sec2=0;
	}
	if(count_min1==10)
	{
		count_min2++;
		count_min1=0;
	}
	if(count_min2==6)
	{
		count_hour1++;
		count_min2=0;
	}
	if(count_hour1==10)
	{
		count_hour2++;
		count_hour1=0;
	}
	if(count_hour2==6)
	{
		count_sec1=0;
		count_sec2=0;
		count_min1=0;
		count_min2=0;
		count_hour1=0;
		count_hour2=0;
	}
}
void seg()

{
	PORTA=(1<<0);
	PORTC=count_sec1;
	_delay_ms(2);
	PORTA=(1<<1);
	PORTC=count_sec2;
	_delay_ms(2);
	PORTA=(1<<2);
	PORTC=count_min1;
	_delay_ms(2);
	PORTA=(1<<3);
	PORTC=count_min2;
	_delay_ms(2);
	PORTA=(1<<4);
	PORTC=count_hour1;
	_delay_ms(2);
	PORTA=(1<<5);
	PORTC=count_hour2;
	_delay_ms(2);
}
void interrupt0()
{
	DDRD&=(~(1<<PD2));
	PORTD|=(1<<PD2);
	MCUCR |= (1<<ISC01);
	GICR  |= (1<<INT0);
}
ISR(INT0_vect)
{
	SREG|=(1<<7);
	TCCR1B |=(1<<CS10)|(1<<CS12);
	if(!(PIND & (1<<PD2)))
	{
		_delay_ms(30);
		if(!(PIND	 & (1<<PD2)))
		{
			count_sec1=0;
			count_sec2=0;
			count_min1=0;
			count_min2=0;
			count_hour1=0;
			count_hour2=0;
		}
	}
}
void interrupt1()
{
	DDRD  &= (~(1<<PD3));
	MCUCR |= (1<<ISC11)|(1<<ISC10);   ;
	GICR  |= (1<<INT1);
}

ISR(INT1_vect)
{
	SREG|=(1<<7);
	if((PIND & (1<<PD3)))
	{
		_delay_ms(30);
		if((PIND&(1<<PD3))) //second check due to switch de-bouncing
		{
			TCCR1B &=~ (1<<CS10)&~(1<<CS12);
		}

	}
}

void interrupt2()
{
	DDRB  &= (~(1<<PB2));
	GICR  |= (1<<INT2);
}
ISR(INT2_vect)
{
	SREG|=(1<<7);
	TCCR1B |=(1<<CS10)|(1<<CS12);
	if(!(PINB & (1<<PB2)))
	{
		_delay_ms(30);
		if(!(PINB	 & (1<<PB2)))
		{
			TCCR1B |=(1<<CS10)|(1<<CS12);
		}
	}
}
int main()
{
	for(i=0;i<4;i++)
		DDRC|=(1<<i);

	for(i=0;i<6;i++)
		DDRA|=(1<<i);
	DDRD&=~(1<<2);
	DDRD&=~(1<<3);
	DDRB&=~(1<<2);
	PORTA=0x01;
	PORTC=0;
	SREG|=(1<<7);
	timer();
	interrupt0();
	interrupt1();
	interrupt2();
	while(1)
	{
		seg();
	}
}
