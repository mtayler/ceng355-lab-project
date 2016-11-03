#ifndef __LCD_H__
#define __LCD_H__

#include "stm32f0xx_conf.h"

/** Wiring
 * PC1 - LCK -> M25
 * PB5 - MOSI -> M17
 * PB3 - SCK -> M21
 */

void lcd_init(void);

void lcd_clear(void);

void lcd_cmd(uint8_t data);

void lcd_char(char c);

#endif //__LCD_H__
