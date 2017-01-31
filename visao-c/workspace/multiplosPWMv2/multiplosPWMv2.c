/* -----------------------------------------------------------------------------
 * Project:		Multiplos PWM v2
 * File:		multiplosPWMv2.c
 * Author:		Sergio L Martins Jr
 * Created:		2017-01-13
 * Modified:	2017-01-13
 * Version:		1.0
 * Purpose:		Programa para controlar multiplos servo motores. Baseado no ApplicationNote Atmel-8020
 * -------------------------------------------------------------------------- */

// --------------------------------------------------------------------------------
//  System definitions ------------------------------------------------------------

#define F_CPU 16000000UL

// --------------------------------------------------------------------------------
//  Header files ------------------------------------------------------------------

#include "globalDefines.h"
#include "ATmega328.h"

// --------------------------------------------------------------------------------
//  Project definitions -----------------------------------------------------------

#define CHMAX 4						// maximum number of PWM channels
#define DEFAULT_VALUE_PWM 0x80		// default PWM value at start up for all channels

// --------------------------------------------------------------------------------
//  New Data Types ----------------------------------------------------------------

typedef union flags{
	struct{
		uint8 unusedBits		: 8;
	};
	uint8 allFlags;
} flags_t;

// --------------------------------------------------------------------------------
//  Global variables --------------------------------------------------------------

volatile flags_t dataFlag;
unsigned char compare[CHMAX];
volatile unsigned char comp_buff[CHMAX];

// --------------------------------------------------------------------------------
// Functions ----------------------------------------------------------------------
void init_pwm(void);

// --------------------------------------------------------------------------------
//  Main function -----------------------------------------------------------------

int main(void)
{	
	// Local variables
	
	// Global variables initialization
	
	// PWM initialization
	setMask(DDRB, 0b1111, 0);	// Seta pinos do pwm como saída
	init_pwm();
	
	// Timer1 initialization	
	timer1SetCompareAValue(1250);
	timer1Config(TIMER_B_MODE_FAST_PWM_OCRA, TIMER_A_PRESCALER_OFF);
	timer1ActivateOverflowInterrupt();
	
//setBit(DDRD, PD2);
//clrBit(PORTD, PD2);
	
	sei();
	
	comp_buff[0] = 0x3F;
	comp_buff[1] = 0x7E;
	comp_buff[2] = 0xBD;
	comp_buff[3] = 23;
    while(1){
		;
    }
}

// --------------------------------------------------------------------------------
//  Handlers ----------------------------------------------------------------------

ISR(TIMER1_OVF_vect){
	static unsigned char softcount = 0xFF;
		
	if (++softcount == 0) {       // increment modulo 256 counter and update
		// the compare values only when counter = 0.
		compare[0] = comp_buff[0]; // verbose code for speed
		compare[1] = comp_buff[1];
		compare[2] = comp_buff[2];
		compare[3] = comp_buff[3]; // last element should equal CHMAX - 1

		setMask(PORTB, 0b1111, 0);	// Set all pwms to high
	}
	
	// clear port pin on compare match (executed on next interrupt)
	if (compare[0] == softcount)
		clrBit(PORTB, PB0);
	if (compare[1] == softcount)
		clrBit(PORTB, PB1);
	if (compare[2] == softcount)
		clrBit(PORTB, PB2);
	if (compare[3] == softcount)
		clrBit(PORTB, PB3);

}

void init_pwm(void)
{
	unsigned char i, pwm;
	
	pwm = DEFAULT_VALUE_PWM;
	
	for(i=0; i<CHMAX; i++){
		compare[i] = pwm;	// set default PWM values
		comp_buff[i] = pwm;
	}
}