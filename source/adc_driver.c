/*
 * adc_driver.c
 *
 *
 *  Created on: Dec 10, 2018
 *      Author: Roberto Baquerizo, Vance Farren
 */
#include "adc_driver.h"

/* Initialize ADC0  in differential mode and
 * with DMA Connection
 *
 * @param[in]:		void
 * @returns:			void
 */
void adc_init( void )
{
	// Enable clocks
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;	// Attach clock to ADC0

	// Configure ADC
	ADC0_CFG1 = 0;	// Clear Configuration Register 1
	ADC0_CFG1 |= (ADC_CFG1_MODE(3)			| /* 16-bit mode */
								ADC_CFG1_ADICLK(0)		| /* Bus Clock */
								ADC_CFG1_ADIV(2)			|	/* Clock divide by 4*/
								ADC_CFG1_ADLSMP_MASK);	/* Long Sample Time */

	ADC0_SC1A	|= ADC_SC1_DIFF_MASK;	/* Enable Differential Mode */

	ADC0_SC3 |= (ADC_SC3_AVGE_MASK	|	/* Enable HW averaging */
							 ADC_SC3_ADCO_MASK);	/* Continuous Sampling */


	ADC0_SC2 &= ADC_SC2_ADTRG_MASK;

	/* DMA Enable */
	ADC0_SC2 |= ADC_SC2_DMAEN_MASK;

	return;
}
