#ifndef __LCD_H__
#define __LCD_H__

#include "stm32f0xx_conf.h"

#define MAX_DIGITS (4)

/* Maximum required delay between SPI writes */
#define MAX_DELAY ((uint32_t)96000)
/* Delay between characters */
#define CHAR_DELAY ((uint32_t)4800)

/** Wiring
 * PC2 - LCK -> M25
 * PB5 - MOSI -> M17
 * PB3 - SCK -> M21
 */

void lcd_init(void);

void lcd_clear(void);

void lcd_cmd(uint8_t data);

void lcd_char(char c);

char* num_to_ascii(uint32_t num);

#endif //__LCD_H__
