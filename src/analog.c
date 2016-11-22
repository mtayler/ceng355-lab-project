#include "analog.h"

#include "diag/Trace.h"
#include "lcd.h"

static uint32_t current_count = 0;
static uint16_t first_edge = 1;
static uint16_t adc_value;

uint16_t adc_read(void) {
	return ADC1->DR;
}

void adc_enable_pot(uint8_t state) {
	if (state) {
		GPIOC->BSRR = GPIO_BSRR_BS_1;
	}
	else {
		GPIOC->BRR = GPIO_BRR_BR_1;
	}
}

void dac_write(uint16_t value) {
	/* Write the provided value to the 12-bit right-aligned DAC input */
	DAC->DHR12R1 = (value & DAC_DHR12R1_DACC1DHR);
}

float freq_read(void) {
	return (TIMER_CLOCK_FREQ)/(float)current_count;
}

/* Initialize the ADC
 */
void adc_init(void) {
	/* Enable clock for GPIOC */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Configure PC1 as push-pull output  */
	GPIOC->MODER |= GPIO_MODER_MODER1_0;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_1;
	/* Ensure high-speed mode for PC1 */
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

/* Initialize the DAC
 */
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

void freq_init(void) {
	/* Enable the GPIOA clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	/* Configure PA1 as an input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);
	/* Ensure no pull up/down for PA1 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	/* Enable the TIM2 clock */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	/* Configure TIM2 with buffer auto-reload, count up,
	 * stop on overflow, enable update events, and interrupt
	 * on overflow only
	 */
	TIM2->CR1 = 0x8C;

	/* Set clock prescaler value */
	TIM2->PSC = TIM2_PRESCALER;
	/* Set auto-reload delay */
	TIM2->ARR = TIM2_AUTORELOAD_DELAY;
	/* Set timer update configuration (rising edge, etc.) */
	TIM2->EGR = (TIM2->EGR & ~0x5F) | (0x1 & 0x5F);

	/* Assign TIM2 interrupt priority 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);
	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);
	/* Enable update interrupt generation */
	TIM2->DIER |= 0x41;

	/* Map EXTI1 line to PA1 */
	SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~(0xF)) | (0 & 0xF);

	/* EXTI1 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= 0x2;

	/* Unmask interrupts from EXTI1 line */
	EXTI->IMR |= 0x2;

	/* Assign EXTI1 interrupt priority 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	/* Enable EXTI1 interrupts in the NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void TIM2_IRQHandler() {
	/* Check if update interrupt flag is set */
	if (TIM2->SR & TIM_SR_UIF) {
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		TIM2->SR |= 0x1;
		/* Restart stopped timer */
		TIM2->CR1 |= 0x1;
	}
}

void EXTI0_1_IRQHandler() {
	/* Check if EXTI1 interrupt pending flag is set */
	// Disable interrupts while servicing
	NVIC_DisableIRQ(EXTI0_1_IRQn);
	if (EXTI->PR & EXTI_PR_PR1) {
		if (first_edge) {
			first_edge = 0;
			/* Reset current timer count */
			TIM2->CNT = 0;
			/* Start the timer */
			TIM2->CR1 |= 0x1;
		} else {
			/* Stop the timer */
			TIM2->CR1 &= ~0x1;
			/* Read the current timer count */
			current_count = TIM2->CNT;

			uint32_t freq_value = (uint32_t)freq_read();
			char* freq_ascii = num_to_ascii(freq_value);
			// Write the frequency value to the LCD
			lcd_cmd(0x82); // Set address to 02
			for (int i = 0; i < 4; i++){
				lcd_char(*(freq_ascii + i));
			}
			// Here we want to obtain the resistance, send the result to the DAC
			// and print it to the LCD. A short wait time will also be added so the display doesn't flicker too much
			adc_value = adc_read();
			dac_write(adc_value); // Send value of ADC to DAC
			char* resistance_ascii = num_to_ascii(adc_value);
			// Write the resistance value to the LCD
			lcd_cmd(0xC2); // Set address to h42
			for (int i = 0; i < MAX_DIGITS; i++) {
				lcd_char(*(resistance_ascii + i));
			}
			// trace_printf("Frequency: %6f [Hz]\n", freq_value);
			first_edge = 1;
		}

		/* Clear the interrupt flag */
		EXTI->PR = EXTI_PR_PR1;
	}

	NVIC_EnableIRQ(EXTI0_1_IRQn);
}
