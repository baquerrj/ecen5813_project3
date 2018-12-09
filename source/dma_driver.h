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

#define DMA_ENTRY_SIZE	(sizeof(uint8_t))
#define DMA_ENTRIES			(4)
#define DMA_BUFFER_SIZE (DMA_ENTRIES * DMA_ENTRY_SIZE)

/* Configure DMA for ADC Operation
 * @param[in]:	void
 * @returns:		void
 */
void dma_init( void );

#endif /* DMA_DRIVER_H_ */
