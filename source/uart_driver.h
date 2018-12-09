/*
 * RES: https://github.com/niekiran/MasteringMCU.git
 *
 *
 *
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "memory_map.h"
#include "ring.h"
#include "settings.h"

#define UART_RDRF 0x20
#define UART_TC   0x40
#define UART_TDRE 0x80

typedef enum
{
  UART_STATE_RESET       = 0x00,    /*!< Peripheral is not yet Initialized                  */
  UART_STATE_READY       = 0x01,    /*!< Peripheral Initialized and ready for use           */
  UART_STATE_BUSY        = 0x02,    /*!< an internal process is ongoing                     */
  UART_STATE_BUSY_TX     = 0x03,    /*!< Data Transmission process is ongoing               */
  UART_STATE_BUSY_RX     = 0x04,    /*!< Data Reception process is ongoing                  */
} uart_state_t;

typedef struct
{
	UART0_Type*			instance;	/* UART registers base address */
	uart_state_t		state;			/* Holds current state of UART Peripheral as defined in uart_stat_t */
} uart_handle_t;

/* Function to check whether the receiver is
 * available to receive a new character
 *
 * @param[in]:	void
 * @returns:		void
 */
void uart_rx_available( void );

/* Function called by ISR to receive character
 *
 * @param[in]:	void
 * @returns:		void
 */
char uart_rx_char( void );

/* Function to check whether the transmitter is available
 * to accept a new character for transmission
 *
 * @param[in]:	void
 * @returns:		void
 */
void uart_tx_available( void );

/* Function called by ISR to transmit a
 * character
 *
 * @param[in]:	c			character to send
 * @returns:		void
 */
void uart_tx_char( char c );

/* Initializes UART Peripheral in Interrupt Mode
 * @param[in]:	void
 * @returns:		void
 */
void uart_init( void );

/* Writes string pointed to p_message
 * Calls uart_tx_char() to transmit character
 *
 * @param[in]:	p_message pointer to message to send
 * @returns:		void
 */
void write( char* p_message );

/* Enable UART0-DMA Connection
 *
 * @param[in]:	void
 * @returns:		void
 */
void enable_UART0_DMA_request( void );

#endif /* UART_DRIVER_H */
