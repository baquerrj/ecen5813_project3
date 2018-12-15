/*
 * dma_driver.c
 *
 *	Reference for absolute value calculation:
 *		https://stackoverflow.com/questions/664852/which-is-the-fastest-way-to-get-the-absolute-value-of-a-number
 *  Created on: Dec 8, 2018
 *      Author: Roberto Baquerizo
 */

#include "dma_driver.h"
#include "uart_driver.h"
#include "adc_driver.h"
#include "led.h"
#include "gpio.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

dma_buffer_s* dma_buffer;

/* Configure DMA for ADC Operation
 * @param[in]:	void
 * @returns:		void
 */
void dma_init( void )
{
	/* Enable Clocks */
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

#if _DUAL_BUFFER_ == 1
	/* Disable DMA Mux channel first */
	DMAMUX0_CHCFG0 = 0x00;
	DMAMUX0_CHCFG1 = 0x00;

	/* Clear pending errors */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;
	DMA_DSR_BCR1 |= DMA_DSR_BCR_DONE_MASK;

	/* Configure DMA with ADC0 as Source */
	DMA_SAR0 =(uint32_t)&ADC0_RA;
	DMA_SAR1 =(uint32_t)&ADC0_RA;

	/* Configure 64-byte transfer - half the buffer */
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(128);
	DMA_DSR_BCR1 = DMA_DSR_BCR_BCR(128);

	/* Clear Source size and Destination size fields */
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);
	DMA_DCR1 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

	/* Set DMA0 with channel linking to DMA1 */

	/* Set DMA0 */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							DMA_DCR_SSIZE(2)	  |		/* Set source size to 16-bits for ADC0_RA */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(3)	  	|   /* Destination address modulo of 64 bytes */
							DMA_DCR_DSIZE(2)		|		/* Set destination to 16-bits to hold ADC0_RA
							 	 	 	 	 	 	 	 	 	 	 	 	 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							DMA_DCR_D_REQ_MASK 	|		/* DMA request is cleared */
							DMA_DCR_LINKCC(3)   |		/* Enable Channel Linking - triggered when BCR reaches 0 */
							DMA_DCR_LCH1(1));				/* Enable Linking to DMA Channel 1 */

	/* Set DMA1 */
	DMA_DCR1 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							DMA_DCR_SSIZE(2)	  |		/* Set source size to 16-bits for ADC0_RA */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(3)	  	|   /* Destination address modulo of 64 bytes */
							DMA_DCR_DSIZE(2)		|		/*Set destination to 16-bits to hold ADC0_RA
							 	 	 	 	 	 	 	 	 	 	 	 	 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							DMA_DCR_D_REQ_MASK  |		/* DMA request is cleared */
							DMA_DCR_LINKCC(3)   |		/* Enable Channel Linking - triggered when BCR reaches 0 */
							DMA_DCR_LCH1(0));				/* Enable Linking to DMA Channel 0 */
#else
	/* Disable DMA Mux channel first */
	DMAMUX0_CHCFG0 = 0x00;

	/* Clear pending errors */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	/* Configure DMA with ADC0 as Source */
	DMA_SAR0 =(uint32_t)&ADC0_RA;
	/* Configure 128-byte transfer */
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(128);


	/* Clear Source size and Destination size fields */
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

	/* Set DMA0 */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK		|		/* Enable interrupt */
							 DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							 DMA_DCR_CS_MASK 			|		/* Single read/write per request */
							 DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							 DMA_DCR_SSIZE(2)	  	|		/* Set source size to 16-bits for ADC0_RA */
							 DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							 DMA_DCR_DMOD(4)	  	|   /* Destination address modulo of 128 bytes */
							 DMA_DCR_DSIZE(2)			|		/* Set destination to 16-bits to hold ADC0_RA
							 													 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							 DMA_DCR_D_REQ_MASK);	  	/* DMA request is cleared */
#endif

	/* Allocate memory for buffer */
	dma_buffer = malloc( sizeof( dma_buffer_s ) );
	/* Set Destination address */
	DMA_DAR0 = (uint32_t)&dma_buffer->buffer;

	/* Enable DMA Channel 0 and select ADC0_RA as Source */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);

#if _DUAL_BUFFER_ == 1
	/* Set Destination address as the top half of the buffer */
	DMA_DAR1 = (uint32_t)&dma_buffer->buffer[DMA_BUFFER_SIZE/2];

	/* Enable DMA Channel 1 and select ADC0_RA as Source */
	DMAMUX0_CHCFG1 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);
#endif

	return;
}

void DMA0_IRQHandler( void )
{
	//PTE5_TOGGLE;
	RED_TOGGLE;
#if _DUAL_BUFFER_ == 0
	uint32_t entry;
	static uint16_t peak_level = 0;
	/* Clear Interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	for( int i = 0; i < DMA_BUFFER_SIZE; i++ )
	{
		entry = (uint16_t)(dma_buffer->buffer[i] );

		char tmp[100];
		sprintf( tmp, "entry [0x%x]\r\n", entry );

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
		sprintf( tmp, "PK = %d\n\r", peak_level );
		while( tmp[j] != '\0' )
		{
			uart_tx_char( tmp[j] );
			j++;
		}
	}
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;
	/* Configure DMA to be ready for next interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_BCR(128);
	DMA_DCR0 |= (DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK);
#endif
#if _DUAL_BUFFER_ == 1
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	/* Configure DMA1 for next interrupt */
	DMA_DCR1 |= (DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK);
	dma_buffer->lower_half_full = 1;
#endif
	PTE5_TOGGLE;
	//RED_TOGGLE;
	write( "\n\rExiting DMA0_IRQHandler\n\r" );
	return;
}

#if _DUAL_BUFFER_ == 1
void DMA1_IRQHandler( void )
{
	PTE5_TOGGLE;
	//BLUE_TOGGLE;
	DMA_DSR_BCR1 |= DMA_DSR_BCR_DONE_MASK;


	/* Configure DMA0 to for next interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_BCR(128);
	DMA_DCR0 |= (DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK);
	DMA_DSR_BCR1 |= DMA_DSR_BCR_BCR(128);
	dma_buffer->upper_half_full = 1;
	PTE5_TOGGLE;
	//BLUE_TOGGLE;
	write( "\n\rExiting DMA1_IRQHandler\n\r" );
	return;
}
#endif
