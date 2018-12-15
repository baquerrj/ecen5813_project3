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
#include "limits.h"

#if _DUAL_BUFFER_ == 1
extern dma_buffer_s* dma_buffer;
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
	uint16_t entry = 0;
	int16_t peak_level = 0;
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
		while( 1 == dma_buffer->lower_half_full )
		{
			for( int i = 0; i < DMA_BUFFER_SIZE/2; i++ )
			{
				entry = (uint16_t)(dma_buffer->buffer[i] );

				char tmp[100];
				sprintf( tmp, "Buffer[1]	entry [0x%x]\r\n", entry );

				int j = 0;
				while( tmp[j] != '\0' )
				{
					uart_tx_char( tmp[j] );
					j++;
				}
				j = 0;
				/* Peak-level Calculation */
				if( ((entry >> 15 | 1) * entry) > peak_level )
				{
					if( SHRT_MAX != peak_level )
						peak_level++;
				}
				else
				{
					peak_level = DMA_ALPHA_SHIFT(peak_level);
				}
				sprintf( tmp, "Buffer[1]	PK = %d\n\r", peak_level );
				while( tmp[j] != '\0' )
				{
					uart_tx_char( tmp[j] );
					j++;
				}
			}
			dma_buffer->lower_half_full = 0;
		}
		while( 1 == dma_buffer->upper_half_full )
		{
			for( int i = DMA_BUFFER_SIZE/2; i < DMA_BUFFER_SIZE; i++ )
			{
				entry = (uint16_t)(dma_buffer->buffer[i] );
				char tmp[100];
				sprintf( tmp, "Buffer[1]	entry [0x%x]\r\n", entry );

				int j = 0;
				while( tmp[j] != '\0' )
				{
					uart_tx_char( tmp[j] );
					j++;
				}
				j = 0;
				/* Peak-level Calculation */
				if( ((entry >> 15 | 1) * entry) > peak_level )

				{
					if( SHRT_MAX != peak_level )
						peak_level++;
				}
				else
				{
					peak_level = DMA_ALPHA_SHIFT(peak_level);
				}
				sprintf( tmp, "Buffer[2]	PK = %d\n\r", peak_level );
				while( tmp[j] != '\0' )
				{
					uart_tx_char( tmp[j] );
					j++;
				}
			}
			dma_buffer->upper_half_full = 0;
		}
#endif
	}
	return 1;
}

