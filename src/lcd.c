#include "lcd.h"

#define LCD_BAUD_RATE_PRESCALER (16);

void lcd_init(void) {
	/* Enable GPIOB clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	/* Enable GPIOC clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Enable SPI1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/* Set PB3 to alternate function */
	GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER3) | (GPIO_MODER_MODER3 & GPIO_MODER_MODER3_1);
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR3;
	/* Set PB5 to alternate function */
	GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER5) | (GPIO_MODER_MODER5 & GPIO_MODER_MODER5_1);
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR5;
	/* Set PC2 to output */
	GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER2) | (GPIO_MODER_MODER2 & GPIO_MODER_MODER2_0);

	/* Set PC2 to push-pull mode */
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_2;

	/* Enable pull up/down resistors on SPI pins */
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR3;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR5;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR2;

	/* Set SPI pins to high-speed */
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR3;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR5;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2;

	/* Initialize SPI */
	SPI_InitTypeDef SPI_InitStructInfo = {
			.SPI_Direction = SPI_Direction_1Line_Tx,
			.SPI_Mode = SPI_Mode_Master,
			.SPI_DataSize = SPI_DataSize_8b,
			.SPI_CPOL = SPI_CPOL_Low,
			.SPI_CPHA = SPI_CPHA_1Edge,
			.SPI_NSS = SPI_NSS_Soft,
			.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256,
			.SPI_FirstBit = SPI_FirstBit_LSB,
			.SPI_CRCPolynomial = 7
	};
	SPI_Init(SPI1, &SPI_InitStructInfo);

	SPI_Cmd(SPI1, ENABLE);

	/* Set LCD to 4bit configuration */
	lcd_cmd(0x20);
}

void lcd_cmd(uint8_t data) {
	/* Force LCK low */
	GPIOC->BRR |= GPIO_BRR_BR_2;
	/* Wait until SPI1 is ready */
	while(!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
	/* Send the data */
	SPI_SendData8(SPI1, data & 0x0F);
	/* Wait until SPI1 is not busy */
	while(SPI1->SR & SPI_SR_BSY);
	/* Force LCK signal to 1 */
	GPIOC->BSRR |= GPIO_BSRR_BS_2;

	for (int i=0; i < 3000000; i++);

	/* Force LCK low */
	GPIOC->BRR |= GPIO_BRR_BR_2;
	/* Wait until SPI1 is ready */
	while(!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
	/* Send the data */
	SPI_SendData8(SPI1, data & 0xF0);
	/* Wait until SPI1 is not busy */
	while(SPI1->SR & SPI_SR_BSY);
	/* Force LCK signal to 1 */
	GPIOC->BSRR |= GPIO_BSRR_BS_2;

	for (int i=0; i < 3000000; i++);
}

void lcd_char(char c) {
	/* Force LCK low */
	GPIOC->BRR |= GPIO_BRR_BR_2;
	/* Wait until SPI1 is ready */
	while(!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
	/* Write the data */
	SPI_SendData8(SPI1, 0x8 | (c & 0x0F));
	/* Force LCK signal to 1 */
	GPIOC->BSRR |= GPIO_BSRR_BS_2;

	for (int i=0; i < 3000000; i++);

	/* Force LCK low */
	GPIOC->BRR |= GPIO_BRR_BR_2;
	/* Wait until SPI1 is ready */
	while(!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
	/* Write the data */
	SPI_SendData8(SPI1, 0x8 | (c & 0xF0));
	/* Force LCK signal to 1 */
	GPIOC->BSRR |= GPIO_BSRR_BS_2;

	for (int i=0; i < 3000000; i++);
}
