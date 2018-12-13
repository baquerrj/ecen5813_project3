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

void* buffer;

/* Configure DMA for ADC Operation
 * @param[in]:	Pointer to Buffer to use as destination
 * @returns:		void
 */
void dma_init( void )
{
	/* Enable Clocks */
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

	/* Disable DMA Mux channel first */
	DMAMUX0_CHCFG0 = 0x00;

	/* Clear pending errors */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	/* Configure DMA with ADC0 as Source */
	DMA_SAR0 =(uint32_t)&ADC0_RA;

	/* Configure 32-byte transfer */
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(128);

	/* Clear Source size and Destination size fields */
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

	/* Set DMA */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							DMA_DCR_SSIZE(2)	  |		/* Set source size to 16-bits for ADC0_RA */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(4)	  	|   /* Destination address modulo of 128 bytes
														 	 	 	 	 	 * corresponding to a buffer of 16 16-bit entries */
							DMA_DCR_DSIZE(2)		|		/*Set destination to 16-bits to hold ADC0_RA
							 	 	 	 	 	 	 	 	 	 	 	 	 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							DMA_DCR_D_REQ_MASK);		/* DMA request is cleared */

	/* Allocate memory for buffer */
	buffer = malloc( DMA_BUFFER_SIZE );
	/* Set Destination address */
	DMA_DAR0 = (uint32_t)&buffer;

	/* Enable DMA Channel 0 and select ADC0_RA as Source */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);
	return;
}

void DMA0_IRQHandler( void )
{
	PTE5_TOGGLE;
	uint32_t upper;
	uint32_t lower;
	static uint16_t peak_level = 0;
	/* Clear Interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	uint32_t destination_end = (uint32_t)&buffer + DMA_BUFFER_SIZE;

	for( int i = (uint32_t)&buffer; i < destination_end; i += 4 )
	{
		upper = (uint16_t)((*( (uint32_t*)i )) >> 16);
		lower = (uint16_t)((*( (uint32_t*)i )) & 0x0000ffff);
		char tmp[50];
		sprintf( tmp, "upper [0x%x]\r\nlower [0x%x]\r\n", upper, lower );
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
		sprintf( tmp, "PK = %d\n\r", peak_level );
		while( tmp[i] != '\0' )
		{
			uart_tx_char( tmp[i] );
			i++;
		}
	}

	/* Configure DMA to be ready for next interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_BCR(128);
	DMA_DCR0 |= (DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK);
	write( "\n\rExiting DMA_IRQHandler\n\r" );
	PTE5_TOGGLE;
	return;
}
