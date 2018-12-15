/*
 * dma_driver.h
 *
 *  Created on: Dec 8, 2018
 *      Author: Roberto Baquerizo
 */

#ifndef DMA_DRIVER_H_
#define DMA_DRIVER_H_

#include "memory_map.h"
#include "settings.h"
#include <stdint.h>

#define DMA_BUFFER_SIZE			(128)
#define DMA_ALPHA_SHIFT(x) 	(x >> 1)

typedef struct
{
#if _DUAL_BUFFER_ == 1
	uint16_t lower_half_full;
	uint16_t upper_half_full;
#endif
	uint16_t buffer[DMA_BUFFER_SIZE];
} dma_buffer_s;

/* Configure DMA for ADC Operation
 * @param[in]:	void
 * @returns:		void
 */
void dma_init( void );

#endif /* DMA_DRIVER_H_ */
