//#include <MKL25Z4.H>
#include "led.h"


/* Initialize LED's of KL25Z FRDM Board
 *
 * @param[in]:		void
 * @returns:			void
 */
void LED_init( void )
{
	/* Enable clock on PORT B and PORT D */
	SIM_SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK);

	/* Set PORTB 18 pin as GPIO */
	PORTB_PCR18	= PORT_PCR_MUX(1);

	/* Set pins as outputs */
	GPIOB_PDDR 	|= RED_SHIFT;

	/* Turn off Red LED */
	GPIOB_PSOR 	|= RED_SHIFT;

	/* Set PORTB 19 pin as GPIO */
	PORTB_PCR19	= PORT_PCR_MUX(1);

	/* Set pin as output */
	GPIOB_PDDR	|= GREEN_SHIFT;

	/* Turn off Green LED */
	GPIOB_PSOR	|= GREEN_SHIFT;

	/* Set PORTD 1 pin as GPIO */
	PORTD_PCR1	= PORT_PCR_MUX(1);

	/* Set pin as output */
	GPIOD_PDDR	|= BLUE_SHIFT;

	/* Turn off Blue LED */
	GPIOD_PSOR	|= BLUE_SHIFT;
	return;
}
