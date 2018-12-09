#include "uart_driver.h"
#include <stdio.h>
#include <string.h>

uart_handle_t uart_handle;

/* Function to check whether the receiver is
 * available to receive a new character
 *
 * @param[in]:	void
 * @returns:		void
 */
void uart_rx_available( void )
{
	if( UART0_S1 & UART0_S1_RDRF_MASK )
	{
		uart_handle.state = UART_STATE_READY;
	}
	else
	{
		uart_handle.state = UART_STATE_BUSY_RX;
	}
	return;
}

/* Function called by ISR to receive character
 *
 * @param[in]:		void
 * @returns:			void
 */
char uart_rx_char( void )
{
	//uart_handle.state = UART_STATE_RESET;
/*
	while( uart_handle.state == UART_STATE_BUSY_RX )
	{
		uart_rx_available();
	}
*/
	while( !(UART0_S1 & UART0_S1_RDRF_MASK) );

	/* Return received character */
	return UART0_D;
}

/* Function to check whether the transmitter is available
 * to accept a new character for transmission
 *
 * @param[in]:		void
 * @returns:			void
 */
void uart_tx_available( void )
{
	if( UART0_S1 & UART_S1_TDRE_MASK )
	{
		uart_handle.state = UART_STATE_READY;
	}
	else
	{
		uart_handle.state = UART_STATE_BUSY_TX;
	}
	return;
}

/* Function called by ISR to transmit a
 * character
 *
 * @param[in]:	c			character to send
 * @returns:		void
 */
void uart_tx_char( char ch )
{
	//uart_handle.state = UART_STATE_RESET;
/*	while( uart_handle.state == UART_STATE_BUSY_TX )
	{
		uart_tx_available();
	}
*/
	while( !(UART0_S1 & UART0_S1_TDRE_MASK) );

	/* Send character */
	UART0_D = (uint8_t)ch;
	return;
}

/* Initializes UART Peripheral in Interrupt Mode
 * @param[in]:	void
 * @returns:		void
 */
void uart_init( void  )
{
	/* Configure UART Instance */
	uart_handle.instance	= UART0;
	uart_handle.state 		= UART_STATE_READY;

	/* enable clock for UART0 */
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;

	/* use FLL(Frequency Locked Loop) output for UART Baud rate generator */
	SIM_SOPT2 |= 0x04000000;

	/* Disable UART0 before changing configurations */
	UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

	UART0_BDH = 0x00;
	UART0_BDL = 0x17; /* 115200 Baud */
	UART0_C4 	= 0x0F; /* Over Sampling Ratio 16 */
	UART0_C1 	= 0x00; /* 8-bit data and no parity */

	/* enable clock for PORTA */
	SIM_SCGC5 	|= SIM_SCGC5_PORTA_MASK;

	/* Enable UART0_Tx on PTA1 and PTA2 */
	PORTA_PCR2 	= PORT_PCR_MUX(0x2);
	PORTA_PCR1 	= PORT_PCR_MUX(0x2);

#if _NON_BLOCKING_
	/* Enable interrupts */
	UART0_C2	|= UART_C2_RIE_MASK;
#endif
	/* Enable receiver and transmitter */
	UART0_C2 	|= (UART0_C2_RE_MASK | UART0_C2_TE_MASK);

	return;
}

/* Enable UART0-DMA Connection
 *
 * @param[in]:	void
 * @returns:		void
 */
void enable_UART0_DMA_request( void )
{
	// Disable UART0 before managing configuration
	UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

	/* Enable Receiver Interrupt */
	UART0_C2 |= UART0_C2_RIE_MASK;

	/* Enable DMA */
	UART0_C5 |= UART0_C5_RDMAE_MASK;

	/* Enable Receiver and Transmitter */
	UART0_C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK);
	return;
}

/* Writes string pointed to p_message
 * Calls uart_tx_char() to transmit character
 *
 * @param[in]:	p_message pointer to message to send
 * @returns:		void
 */
void write( char* p_message )
{
	while( *p_message )
	{
		uart_tx_char( *p_message++ );
	}
	return;
}

#if _NON_BLOCKING_
/* UART0 interrupt handler */
void UART0_IRQHandler( void )
{
	char c = 0;

	if( UART0_S1 & UART0_S1_RDRF_MASK )
	{
		c = uart_rx_char();
		if( (UART0_S1 & UART0_S1_TDRE_MASK) ||
				(UART0_S1 & UART0_S1_TC_MASK) )
		{
			uart_tx_char( c );
		}
	}
	return;
}
#endif

