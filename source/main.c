/*
 * References:
 * 			https://github.com/niekiran/MasteringMCU
 * 			https://electrovolt.ir/wp-content/uploads/2017/07/Freescale_ARM_Cortex_M_Embedded_ElectroVolt.ir_.pdf
 *
*/
//#include <MKL25Z4.H>
#include "memory_map.h"
#include "uart_driver.h"
#include "dma_driver.h"
#include "LED.h"

extern uart_handle_t uart_handle;

int main( void )
{
	SIM_SCGC5	|= (SIM_SCGC5_PORTA_MASK |
								SIM_SCGC5_PORTB_MASK |
								SIM_SCGC5_PORTC_MASK |
								SIM_SCGC5_PORTD_MASK |
								SIM_SCGC5_PORTE_MASK);
	MCG->C4 = 0x20;
	LED_init();

	/* UART0 Init */
	uart_init();
	write( "UART0 is initialized\r\n" );

	__disable_irq();        /* global disable IRQs */
	NVIC_EnableIRQ( DMA0_IRQn );
	dma_init();
	write( "DMA is initialized\r\n" );
	enable_UART0_DMA_request();
	write( "UART0-DMA connection established\r\n" );
	NVIC_EnableIRQ( UART0_IRQn );
	NVIC_SetPriority( UART0_IRQn, 2 );
	__enable_irq();					/* global enable IRQs */

	int red_counter 	= 0;
	int green_counter = 0;
	int blue_counter 	= 0;
	int purp_counter	= 0;
	while (1)
	{
		red_counter  	= 0;
		green_counter	= 0;
		blue_counter 	= 0;
		purp_counter	= 0;
		while( blue_counter <  900000 )
		{
			BLUE_TOGGLE;
		  blue_counter++;
		}
		BLUE_OFF;
		while( green_counter < 300000 )
		{
			GREEN_TOGGLE;
			green_counter++;
		}
		GREEN_OFF;
		while( red_counter < 600000 )
		{
			RED_TOGGLE;
		  red_counter++;
		}
		RED_OFF;

		/* Make purple */
		RED_ON;
		BLUE_ON;
		while( purp_counter < 1000000 )
		{
			purp_counter++;
		}
		RED_OFF;
	}
	return 1;
}

