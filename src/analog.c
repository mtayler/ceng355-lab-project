#include "analog.h"

void adc_init(void) {
	/* Enable clock for GPIOC */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Configure PC1 as push-pull output  */
	GPIOC->MODER |= GPIO_MODER_MODER1_0;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_1;
	/* Ensure high-speed mode for PC8 */
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR1;
	/* Disable any pull up/down resistors on PC1 */
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR1;

	/* Configure PA0 as an analog pin */
	GPIOA->MODER |= GPIO_MODER_MODER0;

	/* Enable the HSI14 (ADC async) clock */
	RCC->CR |= RCC_CR_HSION;
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	/* Start up the ADC */
	ADC1->CR = ADC_CR_ADEN;

	/* ---- Configure the ADC ---- */
	/* Set the ADC clock to the dedicated clock */
	ADC1->CFGR2 &= ~(0x00);
	/* Select the input channel */
	ADC1->CHSELR = ADC_CHSELR_CHSEL0;
	/* Set continuous conversion mode */
	ADC1->CFGR1 |= ADC_CFGR1_CONT;
	/* Enable starting with software trigger */
	ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;
	/* Disable auto-off */
	ADC1->CFGR1 &= ~ADC_CFGR1_AUTOFF;
	/* ----------------------------*/

	/* Wait for the ADC to stabilize */
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	/* Start converting the analog input */
	ADC1->CR |= ADC_CR_ADSTART;
}

/* Enable or disable the potentiometer value line
 * Inputs:
 * 	state: 1 for enable 0 for disable
 */
void adc_enable_pot(uint8_t state) {
	if (state) {
		GPIOC->BSRR = GPIO_BSRR_BS_1;
	}
	else {
		GPIOC->BRR = GPIO_BRR_BR_1;
	}
}

/* Read the current value for the potentiometer
 * If POT ENABLE is not high this will return garbage results
 * Outputs:
 * 	uint16_t: right-aligned 12-bit ADC value
 */
uint16_t adc_read(void) {
	return ADC1->DR;
}

void dac_init(void) {
	/* Set PA4 (DAC output) as analog output pin */
	GPIOA->MODER = GPIO_MODER_MODER4;
	/* Set PA4 to open drain */
	GPIOA->OTYPER = GPIO_OTYPER_OT_4;
	/* Disable pull up/down resistors on PA4 */
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;
	/* Set PA4 to high-speed mode */
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4;

	/* Enable the DAC clock */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	/* Enable the DAC (with output buffering and auto-triggering */
	DAC->CR = DAC_CR_EN1;
}

/* Write the output value of the DAC
 * Inputs:
 * 	uint16_t value: 12-bit right-aligned value to set
 */
void dac_write(uint16_t value) {
	/* Write the provided value to the 12-bit right-aligned DAC input */
	DAC->DHR12R1 = (value & DAC_DHR12R1_DACC1DHR);
}
