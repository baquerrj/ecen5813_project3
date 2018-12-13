/*
 * References:
 * 			https://github.com/niekiran/MasteringMCU
 * 			https://electrovolt.ir/wp-content/uploads/2017/07/Freescale_ARM_Cortex_M_Embedded_ElectroVolt.ir_.pdf
 *
*/
#include "memory_map.h"
#include "uart_driver.h"
#include "dma_driver.h"
#include "adc_driver.h"
#include "LED.h"
#include "gpio.h"

int main( void )
{
	SIM_SCGC5	|= (SIM_SCGC5_PORTA_MASK |
								SIM_SCGC5_PORTB_MASK |
								SIM_SCGC5_PORTC_MASK |
								SIM_SCGC5_PORTD_MASK |
								SIM_SCGC5_PORTE_MASK);
	MCG->C4 = 0x20;

	/* LED Init */
	LED_init();

	/* PORTE PIN 5 Init as GPIO */
	gpio_init();

	/* UART0 Init */
	uart_init();
	write( "UART0 is initialized\r\n" );

	__disable_irq();        /* global disable IRQs */
	NVIC_EnableIRQ( DMA0_IRQn );

	/* DMA Init */
	dma_init();
	write( "DMA is initialized\r\n" );

	/* ADC0 Init */
	adc_init();
	write( "ADC0-DMA connection established\r\n" );

#ifdef _NON_BLOCKING_
	enable_UART0_DMA_request();
	write( "UART0-DMA connection established\r\n" );
	NVIC_EnableIRQ( UART0_IRQn );
	NVIC_SetPriority( UART0_IRQn, 2 );
#endif

	__enable_irq();					/* global enable IRQs */
	while (1)
	{
		/* start conversion on channel 26 temperature */
		ADC0_SC1A = 0x20;
		while(!(ADC0_SC1A & 0x80)) { } /* wait for COCO */

	}
	return 1;
}

