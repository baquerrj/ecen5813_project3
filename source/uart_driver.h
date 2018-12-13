/*
 * uart_driver.h
 * RES: https://github.com/niekiran/MasteringMCU.git
 *
 *  Created on: Dec 5, 2018
 *      Author: Roberto Baquerizo, Vance Farren
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "memory_map.h"
#include "settings.h"

/* Receives a character from UART0
 *
 * @param[in]:	void
 * @returns:		charm received
 */
char uart_rx_char( void );

/* Transmist a character using UART0
 *
 * @param[in]:	c	char to transmit
 * @returns:		void
 */
void uart_tx_char( char c );

/* Initializes UART Peripheral in Interrupt Mode
 * @param[in]:	void
 * @returns:		void
 */
void uart_init( void );

/* Enable UART0-DMA Connection
 *
 * @param[in]:	void
 * @returns:		void
 */
void enable_UART0_DMA_request( void );

/* Writes string pointed to p_message
 * Calls uart_tx_char() to transmit character
 *
 * @param[in]:	p_message pointer to message to send
 * @returns:		void
 */
void write( char* p_message );

#endif /* UART_DRIVER_H */
