#ifndef __ANALOG_H__
#define __ANALOG_H__

#include <stdio.h>
#include <stdlib.h>

#include "stm32f0xx_conf.h"

/* No prescaler on timer 2 */
#define TIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for auto-reload */
#define TIM2_AUTORELOAD_DELAY ((uint32_t)0xFFFFFFFF)
/* 48MHz clock speed */
#define TIMER_CLOCK_FREQ ((uint32_t)48000000)

/* Wiring:
 * 	Potentiometer/ADC:
 *		- ADC input POT M20 -> PA0
 *		- POT_EN M32 -> PC1
 *	DAC:
 *		- DAC output is PA4
 *	Freq. Measurement:
 *		- Input is PA1
 */

void adc_init(void);
void dac_init(void);
void freq_init(void);

/* Enable or disable the potentiometer value line
 * Inputs:
 * 	state: 1 for enable 0 for disable
 */
void adc_enable_pot(uint8_t state);

/* Read the current value for the potentiometer
 * If POT ENABLE is not high this will return garbage results
 * Outputs:
 * 	uint16_t: right-aligned 12-bit ADC value
 */
uint16_t adc_read(void);

/* Write the output value of the DAC
 * Inputs:
 * 	uint16_t value: 12-bit right-aligned value to set
 */
void dac_write(uint16_t value);

/* Returns the current input frequency
 * Outputs:
 * 	float: current frequency in Hz
 */
uint32_t period_to_freq(uint32_t count);

#endif // __ANALOG_H__
