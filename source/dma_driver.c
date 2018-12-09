/*
 * dma_driver.c
 *
 *  Created on: Dec 8, 2018
 *      Author: Roberto Baquerizo
 */

#include "dma_driver.h"
#include "uart_driver.h"
#include <stdlib.h>

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
	//DMA_SAR0 =(uint32_t)&ADC0_RA;
	/* Set UART0_D as Source */
	DMA_SAR0 = (uint32_t)&UART0_D;

	/* Configure 4-byte transfer */
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(4);

	/* Clear Source size and Destination size fields */
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

	/* Set DMA */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
						//DMA_DCR_SSIZE(2)	  |		/* Ser source size to 16 bits for ADC */
							DMA_DCR_SSIZE(1)		|		/* Set source size to 8 bits */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(4)	  	|   /* Destination address modulo of 32 bytes */
							DMA_DCR_DSIZE(1)		|		/* Set destination size of 8 bits */
							DMA_DCR_D_REQ_MASK);		/* DMA request is cleared */


	/* Allocate memory for buffer */
	buffer = malloc( DMA_BUFFER_SIZE );
	/* Set Destination address */
	DMA_DAR0 = (uint32_t)&buffer;

	/* Enable DMA Channel 0 and select ADC0_RA as Source */
	// DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);
	/* Enable DMA Channel 0 and select UART0_D as Source */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(2);

#if _NON_BLOCKING_
	/* Enable interrupt */
	NVIC_EnableIRQ(DMA0_IRQn);
#endif

	return;
}

void DMA0_IRQHandler( void )
{
	uint32_t word;
	/* Clear Interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;
	uint32_t destination_end = (uint32_t)&buffer + DMA_BUFFER_SIZE;
	for( int i = (uint32_t)&buffer; i < destination_end; i += DMA_ENTRIES )
	{
		word = (*( (uint32_t*)i ));
		for( int j = 0; j < DMA_ENTRIES; j += DMA_ENTRY_SIZE )
		{
			uart_tx_char( word );
			word = word >> 8;
		}
	}
	write( "DMA request disabled\r\n" );
	return;
}
