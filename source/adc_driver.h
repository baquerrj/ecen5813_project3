/*
 * adc_driver.h
 *
 *  Created on: Dec 10, 2018
 *      Author: Roberto Baquerizo, Vance Farren
 */

#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#include "memory_map.h"

/* Initialize ADC0  in differential mode and
 * with DMA Connection
 *
 * @param[in]:		void
 * @returns:			void
 */
void adc_init( void );


#endif /* ADC_DRIVER_H_ */

