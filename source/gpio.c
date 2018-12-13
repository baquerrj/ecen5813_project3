/*
 * gpio.c
 *
 *  Created on: Dec 11, 2018
 *      Author: Roberto Baquerizo
 */


#include "gpio.h"

/* Enable PORTE Pin 5 as PGIO
 *
 * @param[in]:		void
 * @returns				void
 */
void gpio_init( void )
{
	/* Enable clock on PORT E */
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

	/* Set PORTE 5 pin as GPIO */
	PORTE_PCR5 = PORT_PCR_MUX(1);

	/* Set pin 5 as output */
	GPIOE_PDDR |= PTE5_SHIFT;

	/* Set PORTB pin 8 high */
	GPIOE_PSOR |= PTE5_SHIFT;

	return;
}
