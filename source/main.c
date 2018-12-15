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
#include <stdio.h>

#if _DUAL_BUFFER_ == 1
extern void* buffer;
extern volatile uint8_t lower_half_full;
extern volatile uint8_t upper_half_full;
#endif

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
#if _DUAL_BUFFER_ == 1
	NVIC_EnableIRQ( DMA1_IRQn );
	uint32_t destination_end = 0;
	uint32_t destination_start = 0;
	uint32_t upper = 0;
	uint32_t lower = 0;
	static uint16_t peak_level = 0;
#endif

	/* ADC0 Init */
	adc_init();
	write( "ADC0-DMA connection established\r\n" );

#if _NON_BLOCKING_ == 1
	//enable_UART0_DMA_request();
	//write( "UART0-DMA connection established\r\n" );
	NVIC_EnableIRQ( UART0_IRQn );
	NVIC_SetPriority( UART0_IRQn, 2 );
#endif

	__enable_irq();					/* global enable IRQs */
	while (1)
	{
		/* start conversion on channel 26 temperature */
		ADC0_SC1A = 0x20;
		while(!(ADC0_SC1A & 0x80)) { } /* wait for COCO */

#if _DUAL_BUFFER_ == 1
		while( lower_half_full )
		{
			destination_end = (uint32_t)&buffer + DMA_BUFFER_SIZE/2;

			for( int i = (uint32_t)&buffer; i < destination_end; i += 4 )
			{
				upper = (uint16_t)((*( (uint32_t*)i )) >> 16);
				lower = (uint16_t)((*( (uint32_t*)i )) & 0x0000ffff);
				char tmp[100];
				sprintf( tmp, "Buffer[1]	upper [0x%x]\r\n	lower [0x%x]\r\n", upper, lower );
				int i = 0;
				while( tmp[i] != '\0' )
				{
					uart_tx_char( tmp[i] );
					i++;
				}
				i = 0;
				/* Peak-level Calculation */
				if( (((upper >> 15 | 1) * upper) > peak_level) |
					(((lower >> 15 | 1) * lower) > peak_level ))
				{
					peak_level++;
				}
				else
				{
					peak_level = DMA_ALPHA_SHIFT(peak_level);
				}
				sprintf( tmp, "Buffer[1]	PK = %d\n\r", peak_level );
				while( tmp[i] != '\0' )
				{
					uart_tx_char( tmp[i] );
					i++;
				}
			}
			lower_half_full = 0;
		}
		while( upper_half_full )
		{
			destination_start = (uint32_t)&buffer + DMA_BUFFER_SIZE/2;
			destination_end = destination_start + DMA_BUFFER_SIZE/2;

			for( int i = destination_start; i < destination_end; i += 4 )
			{
				upper = (uint16_t)((*( (uint32_t*)i )) >> 16);
				lower = (uint16_t)((*( (uint32_t*)i )) & 0x0000ffff);
				char tmp[100];
				sprintf( tmp, "Buffer[2]	upper [0x%x]\r\n	lower [0x%x]\r\n", upper, lower );
				int i = 0;
				while( tmp[i] != '\0' )
				{
					uart_tx_char( tmp[i] );
					i++;
				}
				i = 0;
				/* Peak-level Calculation */
				if( (((upper >> 15 | 1) * upper) > peak_level) |
					(((lower >> 15 | 1) * lower) > peak_level ))
				{
					peak_level++;
				}
				else
				{
					peak_level = DMA_ALPHA_SHIFT(peak_level);
				}
				sprintf( tmp, "Buffer[2]	PK = %d\n\r", peak_level );
				while( tmp[i] != '\0' )
				{
					uart_tx_char( tmp[i] );
					i++;
				}
			}
			upper_half_full = 0;
		}
#endif
	}
	return 1;
}

