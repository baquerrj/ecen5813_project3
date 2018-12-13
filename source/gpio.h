/*
 * gpio.h
 *
 *  Created on: Dec 11, 2018
 *      Author: Roberto Baquerizo
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "memory_map.h"

#define PIN(x)					(x)
#define PTE_SHIFT(x)		(1 << PIN(x))
#define PTE_TOGGLE(x)		(GPIOE_PTOR = PTE_SHIFT(x))

#define PTE5						PTE(5)
#define	PTE5_SHIFT			PTE_SHIFT(5)
#define PTE5_TOGGLE			PTE_TOGGLE(5)

/* Enable PORTE Pin 5 as PGIO
 *
 * @param[in]:		void
 * @returns				void
 */
void gpio_init( void );

#endif /* GPIO_H_ */
