#include "lcd.h"

#define LCD_BAUD_RATE_PRESCALER (16);

/* Output data to the shift register through SPI */
void spi_write(uint8_t);

void lcd_init(void) {
	/* Enable SPI1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	/* Enable GPIOB clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* Enable GPIOC clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Set PC2 to output */
	GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER2) | (GPIO_MODER_MODER2 & GPIO_MODER_MODER2_0);
	/* Set PB3 to alternate function */
	GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER3) | (GPIO_MODER_MODER3 & GPIO_MODER_MODER3_1);
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR3;
	/* Set PB5 to alternate function */
	GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER5) | (GPIO_MODER_MODER5 & GPIO_MODER_MODER5_1);
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR5;

	/* Set PC2, PB3, PB5 to push-pull mode */
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_2;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_5;

	/* Disable pull up/down resistors on SPI pins */
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR3;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR5;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR2;

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
			.SPI_FirstBit = SPI_FirstBit_MSB,
			.SPI_CRCPolynomial = 7
	};
	SPI_Init(SPI1, &SPI_InitStructInfo);

	SPI_Cmd(SPI1, ENABLE);

	/* Set the LCD to 4 bit interface */
	spi_write(0x02);
	spi_write(0x82);
	spi_write(0x02);

	/* Set LCD to display 2 lines */
	lcd_cmd(0x28);

	/* Clear the LCD */
	lcd_cmd(0x01);
	/* Home the cursor */
	lcd_cmd(0x02);
	for (int i=0;i<12000000;i++);
	/* Set cursor move direction and disable display shift */
	lcd_cmd(0x06);
	/* Set the display on, don't show the cursor, don't blink */
	lcd_cmd(0x0C);
	/* Set the cursor to move right, no display shift */
	lcd_cmd(0x14);
	/* Write 'F:    Hz' to first line */
	lcd_cmd(0x80); // Start at very left

	lcd_char(0x46); //F
	lcd_char(0x3A); //:
	/* 4 spaces */
	for (int i = 0; i < 4; i++) {
		lcd_char(0x20);
	}
	lcd_char(0x48); //H
	lcd_char(0x7A); //z
	/* Write 'R:    Oh' to second line*/
	lcd_cmd(0xC0); // set address to second line, first character
	lcd_char(0x52); //R
	lcd_char(0x3A); //:
		/* 4 spaces */
	for (int i = 0; i < 4; i++) {
		lcd_char(0x20);
	}
	lcd_char(0x4F); //O
	lcd_char(0x68); //h
}

void spi_write(uint8_t data) {
	/* Force LCK low */
	GPIOC->BRR |= GPIO_BRR_BR_2;
	/* Wait until SPI1 is ready */
	while((SPI1->SR & SPI_SR_BSY) && ~(SPI1->SR & SPI_SR_TXE));
	/* Send the data */
	SPI_SendData8(SPI1, data);
	/* Wait until SPI1 is not busy */
	while(SPI1->SR & SPI_SR_BSY);
	/* Force LCK signal to 1 */
	GPIOC->BSRR |= GPIO_BSRR_BS_2;
}

/*
 * Write to LCD using 4 bit interface. Send 4 high bits
 * by pulsing EN, then do the same for 4 low bits
 */
void lcd_cmd(uint8_t data) {
	/* Send HIGH bits */
	spi_write(0x00 | (data >> 4));
	spi_write(0x80 | (data >> 4));
	spi_write(0x00 | (data >> 4));
	/* Send LOW bits */
	spi_write(0x00 | (data & 0x0F));
	spi_write(0x80 | (data & 0x0F));
	spi_write(0x00 | (data & 0x0F));

}

void lcd_char(char c) {
	/* Send HIGH bits */
	spi_write(0x40 | ((uint8_t)c >> 4));
	spi_write(0xC0 | ((uint8_t)c >> 4));
	spi_write(0x40 | ((uint8_t)c >> 4));
	/* Send LOW bits */
	spi_write(0x40 | ((uint8_t)c & 0x0F));
	spi_write(0xC0 | ((uint8_t)c & 0x0F));
	spi_write(0x40 | ((uint8_t)c & 0x0F));
}
