/*
 * adc_driver.c
 *
 *  Created on: Dec 10, 2018
 *      Author: Roberto Baquerizo
 */
#include "adc_driver.h"

void adc_init( void )
{
	//suggested flow on p. 501: update CFG1, SC2, SC1n, check in loop if COCO != 1 loopback, else read RN to clear COCO, continue
	SIM->SCGC6 |= 0x8000000; /* attach clock to ADC0, p. 207 in ref manual */ //select clock input src and divide ratio to generate ADCK
	ADC0->SC2 &= ~0x40; /* software trigger p. 470*///select conversion trigger, HW or SW, compare fxns
	ADC0->SC3 =0x0C; //continous/oneshot sampling, HW averaging
	ADC0->SC1[0] = 0x20; //enable differential mode p.464// select single or differential, enable/disable COCO interrupts, select input channel to perform conversions
						//differential inputs: p. 459
						//adc data result p. 468
						//conversion time calculation 489
	//ADC0->SC1 = 0x60; //assign adc0 to differential mode use DADP0 pair p . 464
	/* clock div by 4, long sample time, differential 16 bit, bus clock */
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00; //p. 465 //third param 0x06 for diff input

	/* DMA Enable */
	ADC0->SC2 |= ADC_SC2_DMAEN_MASK;
	return;
}
