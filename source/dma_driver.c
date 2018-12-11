/*
 * dma_driver.c
 *
 *  Created on: Dec 8, 2018
 *      Author: Roberto Baquerizo
 */

#include "dma_driver.h"
#include "uart_driver.h"
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

#if _ADC_
	/* Configure DMA with ADC0 as Source */
	DMA_SAR0 =(uint32_t)&ADC0_RA;
#else
	/* Set UART0_D as Source */
	DMA_SAR0 = (uint32_t)&UART0_D;
#endif

	/* Configure 16-byte transfer */
	DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(16);

	/* Clear Source size and Destination size fields */
	DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

#if _ADC_
	/* Set DMA */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							DMA_DCR_SSIZE(2)	  |		/* Set source size to 16-bits for ADC0_RA */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(1)	  	|   /* Destination address modulo of 32 bytes
														 	 	 	 	 	 * corresponding to a buffer of 8 16-bit entries */
							DMA_DCR_DSIZE(2)		|		/*Set destination to 16-bits to hold ADC0_RA
							 	 	 	 	 	 	 	 	 	 	 	 	 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							DMA_DCR_D_REQ_MASK);		/* DMA request is cleared */
#else
	/* Set DMA */
	DMA_DCR0 |= (DMA_DCR_EINT_MASK	|		/* Enable interrupt */
							DMA_DCR_ERQ_MASK 		|		/* Enable peripheral request */
							DMA_DCR_CS_MASK 		|		/* Single read/write per request */
							DMA_DCR_EADREQ_MASK	| 	/* Enable Async DMA Requests */
							DMA_DCR_SSIZE(1)		|		/* Set source size to 8 bits */
							DMA_DCR_DINC_MASK		|		/* Set increments to destination address */
							DMA_DCR_DMOD(1)	  	|   /* Destination address modulo of 16 bytes
								 	 	 	 	 	 	 	 	 	 	 	 * Seems to me that DMOD should be the number of
								 	 	 	 	 	 	 	 	 	 	 	 * entries in the buffer */
							DMA_DCR_DSIZE(1)		|		/* Set destination size of 8 bits
													 	 	 	 	 	 	 * DSIZE should be the size of a single
							 	 	 	 	 	 	 	 	 	 	 	 	 * entry in the buffer. For my testing
							 	 	 	 	 	 	 	 	 	 	 	 	 * it was 8 bits for uint8_t. Make it
							 	 	 	 	 	 	 	 	 	 	 	 	 * DSIZE(2) for ADC0->R[0]'s 16-bit results */
							DMA_DCR_D_REQ_MASK);		/* DMA request is cleared */
#endif

	/* Allocate memory for buffer */
	buffer = malloc( DMA_BUFFER_SIZE );
	/* Set Destination address */
	DMA_DAR0 = (uint32_t)&buffer;

#if _ADC_
	/* Enable DMA Channel 0 and select ADC0_RA as Source */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);
#else
	/* Enable DMA Channel 0 and select UART0_D as Source */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(2);
#endif
	return;
}

void DMA0_IRQHandler( void )
{
	uint32_t word;
	/* Clear Interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

	uint32_t destination_end = (uint32_t)&buffer + DMA_BUFFER_SIZE;
	for( int i = (uint32_t)&buffer; i < destination_end; i += 4 )
	{
		word = (*( (uint32_t*)i ));
		char tmp[10];
		sprintf( tmp, "%x\r\n", word>>16 );
		int i = 0;
		while( tmp[i] != '\0' )
		{
			uart_tx_char( tmp[i] );
			i++;
		}
	}

	/* Configure DMA to be ready for next interrupt */
	DMA_DSR_BCR0 |= DMA_DSR_BCR_BCR(16);
	DMA_DCR0 |= (DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK);
	write( "\n\rExiting DMA_IRQHandler\n\r" );
	return;
}
