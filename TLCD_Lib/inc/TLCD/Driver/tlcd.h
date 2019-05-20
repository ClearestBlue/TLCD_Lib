/*
 * tlcd.h
 *
 *  Created on: 19 maj 2019
 *      Author: Mariusz
 */

#ifndef TLCD_DRIVER_TLCD_H_
#define TLCD_DRIVER_TLCD_H_


static const uint8_t tlcd_default_width = 128;
// for 1.44" display
static const uint8_t tlcd_default_height_144 = 128;
// for 1.8" display
static const uint8_t tlcd_default_height_18 = 160;

enum TLCD_DISPLAY_TYPE {
	TLCD_BLUE,
	TLCD_RED_18_GREENTAB,
	TLCD_RED_18_REDTAB,
	TLCD_RED_18_BLACKTAB,
	TLCD_RED144_GREENTAB
};

static const enum TLCD_DISPLAY_TYPE tlcd_type = TLCD_RED_18_BLACKTAB;


// TLCD commands
enum TLCD_COMMANDS {
	TLCD_NOP = 0x00,
	TLCD_SWRESET = 0x01,
	TLCD_RDDID = 0x04,
	TLCD_RDDST = 0x09,

	TLCD_SLPIN = 0x10,
	TLCD_SLPOUT = 0x11,
	TLCD_PTLON = 0x12,
	TLCD_NORON = 0x13,

	TLCD_INVOFF = 0x20,
	TLCD_INVON = 0x21,
	TLCD_DISPOFF = 0x28,
	TLCD_DISPON = 0x29,
	TLCD_CASET = 0x2A,
	TLCD_RASET = 0x2B,
	TLCD_RAMWR = 0x2C,
	TLCD_RAMRD = 0x2E,

	TLCD_PTLAR = 0x30,
	TLCD_COLMOD = 0x3A,
	TLCD_MADCTL = 0x36,

	TLCD_FRMCTR1 = 0xB1,
	TLCD_FRMCTR2 = 0xB2,
	TLCD_FRMCTR3 = 0xB3,
	TLCD_INVCTR = 0xB4,
	TLCD_DISSET5 = 0xB6,

	TLCD_PWCTR1 = 0xC0,
	TLCD_PWCTR2 = 0xC1,
	TLCD_PWCTR3 = 0xC2,
	TLCD_PWCTR4 = 0xC3,
	TLCD_PWCTR5 = 0xC4,
	TLCD_VMCTR1 = 0xC5,

	TLCD_RDID1 = 0xDA,
	TLCD_RDID2 = 0xDB,
	TLCD_RDID3 = 0xDC,
	TLCD_RDID4 = 0xDD,

	TLCD_PWCTR6 = 0xFC,

	TLCD_GMCTRP1 = 0xE0,
	TLCD_GMCTRN1 = 0xE1
};

enum TLCD_COLORS {
	TLCD_NO_ALPHA = -0x0001,
	TLCD_COLOR_BLACK = 0x0000,
	TLCD_COLOR_BLUE = 0x001F,
	TLCD_COLOR_RED = 0xF800,
	TLCD_COLOR_GREEN = 0x07E0,
	TLCD_COLOR_CYAN = 0x07FF,
	TLCD_COLOR_MAGENTA = 0xF81F,
	TLCD_COLOR_YELLOW = 0xFFE0,
	TLCD_COLOR_WHITE = 0xFFFF
};

enum TLCD_ORIENTATION {
	TLCD_LANDSCAPE,
	TLCD_PORTRAIT,
	TLCD_LANDSCAPE_INV,
	TLCD_PORTRAIT_INV
};

enum TLCD_TEXT_ALIGNMENTS{
	TLCD_TO_LEFT,
	TLCD_TO_RIGHT
};

typedef struct{
	int16_t x;
	int16_t y;
}Point_t;

typedef struct{
	Point_t p1;
	Point_t p2;
	Point_t p3;
	Point_t p4;
}Box_t;

typedef struct{

	int16_t x;
	int16_t y;
	uint8_t w;
	uint8_t h;
	int32_t alpha;

	uint8_t z;
	uint8_t draw;

	const uint16_t * src;

}Layer_t;

#define MOVE_DELTA_MAX 40
#define BACKGROUND_COLOR 0x8430

void tlcd_init(void);
void tlcd_set_orientation(enum TLCD_ORIENTATION orientation);

void tlcd_draw_pixel(int16_t x, int16_t y, uint16_t color);
void tlcd_fill_rect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint16_t color);

void tlcd_draw_fast_vline( uint8_t x, uint8_t y, uint8_t h, uint16_t color );
void tlcd_draw_fast_hline( uint8_t x, uint8_t y, uint8_t w, uint16_t color );

void tlcd_draw_font_bitmap( int16_t x, int16_t y, const uint8_t * bitmap, uint8_t nr, uint16_t color, uint16_t colorBg );

void tlcd_draw_layer( Layer_t * layer );
void tlcd_add_layer( Layer_t * layer );
void tlcd_set_layer( Layer_t * layer, int16_t x, int16_t y, const uint16_t * src, int32_t alpha );
void tlcd_clear( uint16_t color );

// Experimetnal functions. Use on own risk
void tlcd_draw_bitmap_rle( int16_t x, int16_t y, const uint16_t * bitmap );

#endif /* TLCD_DRIVER_TLCD_H_ */
