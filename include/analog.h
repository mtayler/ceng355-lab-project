#ifndef __ANALOG_H__
#define __ANALOG_H__

#include <stdio.h>
#include <stdlib.h>

#include "stm32f0xx_conf.h"

/* Wiring:
 * 	Potentiometer/ADC:
 *		- ADC input is PA0
 *		- POT ENABLE wired to PC8
 */

void adc_init(void);
void adc_enable_pot(uint8_t state);
uint16_t adc_read(void);

void dac_init(void);
void dac_write(uint16_t value);

void freq_init(void);

#endif // __ANALOG_H__
