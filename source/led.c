//#include <MKL25Z4.H>
#include "led.h"

void LED_init( void )
{
	/* Enable clock on PORT B and PORT D */
	SIM_SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK);
	//SIM->SCGC5 |= 0x400;        /* enable clock to Port B */
	//SIM->SCGC5 |= 0x1000;       /* enable clock to Port D */

	/* Set PORTB 18 pin as GPIO */
	PORTB_PCR18	= PORT_PCR_MUX(1);
	//PORTB->PCR[18] = 0x100;     /* make PTB18 pin as GPIO */

	/* Set pins as outputs */
	GPIOB_PDDR 	|= RED_SHIFT;
//	PTB->PDDR |= 0x40000;       /* make PTB18 as output pin */

	/* Turn on Red LED */
	GPIOB_PSOR 	|= RED_SHIFT;
//	PTB->PSOR = 0x40000;        /* turn off red LED */

	/* Set PORTB 19 pin as GPIO */
	PORTB_PCR19	= PORT_PCR_MUX(1);
//	PORTB->PCR[19] = 0x100;     /* make PTB19 pin as GPIO */

	/* Set pin as output */
	GPIOB_PDDR	|= GREEN_SHIFT;
//	PTB->PDDR |= 0x80000;       /* make PTB19 as output pin */

	/* Turn on Green LED */
	GPIOB_PSOR	|= GREEN_SHIFT;
//	PTB->PSOR = 0x80000;        /* turn off green LED */

	/* Set PORTD 1 pin as GPIO */
	PORTD_PCR1	= PORT_PCR_MUX(1);
//	PORTD->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */

	/* Set pin as output */
	GPIOD_PDDR	|= BLUE_SHIFT;
//	PTD->PDDR |= 0x02;          /* make PTD1 as output pin */

	/* Turn on Blue LED */
	GPIOD_PDDR	|= BLUE_SHIFT;
//	PTD->PSOR = 0x02;           /* turn off blue LED */
	return;
}

void LED_toggle( void )
{
	static int value = 0;
	if( value == 0)    /* use bit 0 of value to control red LED */
	PTB->PCOR = 0x40000;    /* turn on red LED */
	else
	 PTB->PSOR = 0x40000;    /* turn off red LED */
	if (value == 1)    /* use bit 1 of value to control green LED */
	PTB->PCOR = 0x80000;    /* turn on green LED */
	else
	PTB->PSOR = 0x80000;    /* turn off green LED */
	if (value == 2)    /* use bit 2 of value to control blue LED */
	PTD->PCOR = 0x02;       /* turn on blue LED */
	else
	PTD->PSOR = 0x02;       /* turn off blue LED */
	value++;
	if (value == 3) value = 0;
	return;
}
