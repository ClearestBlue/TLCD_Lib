/*
 * tlcd_font.h
 *
 *  Created on: 19 maj 2019
 *      Author: Mariusz
 */

#ifndef TLCD_DRIVER_TLCD_FONT_H_
#define TLCD_DRIVER_TLCD_FONT_H_

typedef struct{

	int16_t x;
	int16_t y;

	uint8_t alignment;

	uint8_t digits;
	uint16_t value;
	char valueStr[16];

}Counter_t;


void tlcd_draw_text( int16_t x, int16_t y, char * text, uint16_t color, uint8_t alignemnt );
void tlcd_draw_counter( Counter_t * cnt, uint16_t color );

#endif /* TLCD_DRIVER_TLCD_FONT_H_ */
