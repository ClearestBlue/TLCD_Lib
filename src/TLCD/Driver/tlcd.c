/*
 * tlcd.c
 *
 *  Created on: 19 maj 2019
 *      Author: Mariusz
 */
#include <stdlib.h>

#include "TLCD/tlcd_config.h"
#include "TLCD/Driver/tlcd.h"
#include "TLCD/Driver/tlcd_cmd.h"

#define STACK_SIZE  16


uint8_t tlcd_row_start;
uint8_t tlcd_column_start;
uint8_t tlcd_width;
uint8_t tlcd_height;
enum TLCD_ORIENTATION tlcd_orientation = TLCD_LANDSCAPE;


Layer_t * stack[STACK_SIZE];
uint8_t stackIndex;
uint16_t bgColor;


static inline void tlcd_write_cmd( enum TLCD_COMMANDS cmd ){

	_TLCD_CLR_RS;

	_TLCD_CLR_CS;
	_TLCD_SPI_TRANSFER( cmd );
	_TLCD_SET_CS;
}

static inline void tlcd_write_data( uint8_t data ){

	_TLCD_SET_RS;

	_TLCD_CLR_CS;
	_TLCD_SPI_TRANSFER( data );
	_TLCD_SET_CS;
}

static inline void tlcd_write_color( uint16_t color ){
	_TLCD_SPI_TRANSFER( color >> 8 );
	_TLCD_SPI_TRANSFER( color & 0xFF );
}


static inline void tlcd_reset( void ){

	_TLCD_CLR_CS;
	_TLCD_SET_RST;
	_TLCD_DELAY_MS( 500 );
	_TLCD_CLR_RST;
	_TLCD_DELAY_MS( 500 );
	_TLCD_SET_RST;
	_TLCD_DELAY_MS( 500 );
}


void tlcd_run_command_list( const uint8_t * addr ){

	uint8_t  cmd_count, arg_count, has_delay;

	cmd_count = _TLCD_PGM_READ_BYTE( addr++ );   						// Number of commands to follow
	for( uint8_t cmd_pos=0; cmd_pos<cmd_count; cmd_pos++ ){
		tlcd_write_cmd( _TLCD_PGM_READ_BYTE( addr++ ) ); 				// Read, send command
		arg_count = _TLCD_PGM_READ_BYTE( addr++ );    				// Number of args to follow
		has_delay = arg_count & DELAY_FLAG;	         			// If set, delay follows args
		arg_count &= ~DELAY_FLAG;                  				// Number of args
		for( uint8_t arg_pos=0; arg_pos<arg_count; arg_pos++ ){ // For each argument...
			tlcd_write_data( _TLCD_PGM_READ_BYTE( addr++ ) );		  	// Read, send argument
		}

		if( has_delay ){
			uint8_t ms;
			ms = _TLCD_PGM_READ_BYTE( addr++ ); 	// Read post-command delay time (ms)
			if( ms == 255 ){  				// If 255, delay for 500 ms
				_TLCD_DELAY_MS( 500 );
			}
			else {
				for( int i=0; i<ms; i+=5 ){
					_TLCD_DELAY_MS( 5 );
				}
			}
		}
	}
}


void tlcd_init(){

	_TLCD_GPIO_INIT();

	tlcd_reset();

	switch( tlcd_type ){
		case TLCD_BLUE:
			tlcd_run_command_list( tlcd_blue_init );
			break;

		case TLCD_RED_18_GREENTAB:
			tlcd_run_command_list( tlcd_red_init1 );
			tlcd_run_command_list( tlcd_red_init_green2 );
			tlcd_run_command_list( tlcd_red_init3 );
			tlcd_column_start = 2;
			tlcd_row_start = 1;
			tlcd_width = tlcd_default_width;
			tlcd_height = tlcd_default_height_18;
			break;

		case TLCD_RED_18_REDTAB:
			tlcd_run_command_list( tlcd_red_init1 );
			tlcd_run_command_list( tlcd_red_init_red2 );
			tlcd_run_command_list( tlcd_red_init3 );
			tlcd_width = tlcd_default_width;
			tlcd_height = tlcd_default_height_18;
			break;

		case TLCD_RED_18_BLACKTAB:
			tlcd_run_command_list( tlcd_red_init1 );
			tlcd_run_command_list( tlcd_red_init_red2 );
			tlcd_run_command_list( tlcd_red_init3 );
			// Change MADCTL color filter for black
			tlcd_write_cmd( TLCD_MADCTL );
			tlcd_write_data( 0xC0 );
			tlcd_width = tlcd_default_width;
			tlcd_height = tlcd_default_height_18;
			break;

		case TLCD_RED144_GREENTAB:
			tlcd_run_command_list( tlcd_red_init1 );
			tlcd_run_command_list( tlcd_red_init_green1442 );
			tlcd_run_command_list( tlcd_red_init3 );
			tlcd_height = tlcd_default_height_144;
			tlcd_column_start = 2;
			tlcd_row_start = 3;
			tlcd_width = tlcd_default_width;
			tlcd_height = tlcd_default_height_144;
			break;
	}
}


enum TLCD_MADCTL_ARGS {
	MADCTL_MY = 0x80,	// Mirror Y
	MADCTL_MX = 0x40,	// Mirrror x
	MADCTL_MV = 0x20,	// Swap XY
	MADCTL_ML = 0x10,	// Scan address order
	MADCTL_RGB = 0x00,
	MADCTL_BGR = 0x08,
	MADCTL_MH = 0x04 	// Horizontal scan oder
};

void tlcd_set_orientation( enum TLCD_ORIENTATION orientation ){

	tlcd_write_cmd( TLCD_MADCTL );

  	switch( orientation ){
		case TLCD_PORTRAIT:
		    if( tlcd_type == TLCD_RED_18_BLACKTAB ){
		    	tlcd_write_data( MADCTL_MX | MADCTL_MY | MADCTL_RGB );
		    }else{
		    	tlcd_write_data( MADCTL_MX | MADCTL_MY | MADCTL_BGR );
		    }

			tlcd_width  = tlcd_default_width;

		    if( tlcd_type == TLCD_RED144_GREENTAB ){
		    	tlcd_height = tlcd_default_height_144;
		    }else{
		    	tlcd_height = tlcd_default_height_18;
			}
			break;


		case TLCD_LANDSCAPE:
			if( tlcd_type == TLCD_RED_18_BLACKTAB ){
				tlcd_write_data( MADCTL_MY | MADCTL_MV | MADCTL_RGB );
			}else{
				tlcd_write_data( MADCTL_MY | MADCTL_MV | MADCTL_BGR );
			}

			if( tlcd_type == TLCD_RED144_GREENTAB ){
				tlcd_width = tlcd_default_height_144;
			}
			else{
				tlcd_width = tlcd_default_height_18;
			}

			tlcd_height = tlcd_default_width;
			break;

		case TLCD_PORTRAIT_INV:
			if( tlcd_type == TLCD_RED_18_BLACKTAB ){
				tlcd_write_data( MADCTL_RGB );
			}else{
				tlcd_write_data( MADCTL_BGR );
			}

			tlcd_width  = tlcd_default_width;

			if( tlcd_type == TLCD_RED144_GREENTAB ){
				tlcd_height = tlcd_default_height_144;
			}else{
				tlcd_height = tlcd_default_height_18;
			}
			break;

		case TLCD_LANDSCAPE_INV:
			if( tlcd_type == TLCD_RED_18_BLACKTAB ){
				tlcd_write_data( MADCTL_MX | MADCTL_MV | MADCTL_RGB );
			}else{
				tlcd_write_data( MADCTL_MX | MADCTL_MV | MADCTL_BGR );
			}

			if( tlcd_type == TLCD_RED144_GREENTAB ){
				tlcd_width = tlcd_default_height_144;
			}else{
				tlcd_width = tlcd_default_height_18;
			}

			tlcd_height = tlcd_default_width;
			break;
	}
}

void tlcd_set_addr_win( uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1 ){

	tlcd_write_cmd( TLCD_CASET ); // Column addr set
	tlcd_write_data( 0x00 );
	tlcd_write_data( x0 + tlcd_column_start );	// XSTART
	tlcd_write_data( 0x00 );
	tlcd_write_data( x1 + tlcd_column_start ); // XEND

	tlcd_write_cmd( TLCD_RASET ); // Row addr set
	tlcd_write_data( 0x00 );
	tlcd_write_data( y0 + tlcd_row_start ); // YSTART
	tlcd_write_data( 0x00 );
	tlcd_write_data( y1 + tlcd_row_start ); // YEND

	tlcd_write_cmd( TLCD_RAMWR ); // write to RAM
}

void tlcd_draw_pixel( int16_t x, int16_t y, uint16_t color ){
	if( x < 0 || x >= tlcd_width || y < 0 || y >= tlcd_height ){
		return;
	}

  	tlcd_set_addr_win( x, y, x+1, y+1 );

  	_TLCD_SET_RS;
  	_TLCD_CLR_CS;

	tlcd_write_color( color );

	_TLCD_SET_CS;
}



void tlcd_fill_rect( int16_t x, int16_t y, uint8_t w, uint8_t h, uint16_t color ){
	if( x >= tlcd_width || y >= tlcd_height ){
		return;
	}

	if( (x + w - 1) >= tlcd_width ){
		w = tlcd_width  - x;
	}
	if( (y + h - 1) >= tlcd_height ){
		h = tlcd_height - y;
	}

	tlcd_set_addr_win( x, y, x + w - 1, y + h - 1 );

  	_TLCD_SET_RS;
  	_TLCD_CLR_CS;

	for( uint8_t i=0; i<h; i++ ){
		for( uint8_t j=0; j<w; j++ ){
			tlcd_write_color( color );
		}
	}

	_TLCD_SET_CS;

}

void tlcd_draw_fast_vline( uint8_t x, uint8_t y, uint8_t h, uint16_t color ){
	tlcd_fill_rect( x, y, 1, h, color );
}

void tlcd_draw_fast_hline( uint8_t x, uint8_t y, uint8_t w, uint16_t color ){
	tlcd_fill_rect( x, y, w, 1, color );
}

void tlcd_draw_font_bitmap( int16_t x, int16_t y, const uint8_t * bitmap, uint8_t nr, uint16_t color, uint16_t colorBg ){

	uint8_t w = 8;
	uint8_t h = 12;

	if( x >= tlcd_width || y >= tlcd_height) return;
	if( (x + w - 1) >= tlcd_width || (y + h - 1) >= tlcd_height ) return;


	tlcd_set_addr_win( x, y, x + w - 1, y + h - 1 );

  	_TLCD_SET_RS;
  	_TLCD_CLR_CS;

	bitmap += nr * 12;

	for( uint8_t i=0; i<h; i++ ){

		uint16_t row = _TLCD_PGM_READ_BYTE( bitmap );

		for( uint8_t j=0; j<w; j++ ){

			uint16_t col;

			if( row >> j & 0x01 ) col = color;
			else{

				col = bgColor;

				for( uint8_t k=0; k<stackIndex; k++ ){

					if( stack[k] ){

						if( !stack[k]->draw ) continue;

						if( ((j + x) >= stack[k]->x) && ((j + x) <= (stack[k]->x + stack[k]->w - 1)) &&
							((i + y) >= stack[k]->y) && ((i + y) <= (stack[k]->y + stack[k]->h - 1)) ){

							int16_t idx = 2 + abs((j + x) - stack[k]->x) + abs((i + y) - stack[k]->y) * stack[k]->w;
							uint16_t new_color = _TLCD_PGM_READ_WORD( stack[k]->src + idx );

							if( new_color != stack[k]->alpha ) col = new_color;

						}
					}

				}
			}

			tlcd_write_color( col );

		}
		bitmap++;
	}

	_TLCD_SET_CS;
}


void tlcd_clear( uint16_t color ){

	tlcd_fill_rect( 0, 0, 128, 160, color );
	bgColor = color;

}

void tlcd_add_layer( Layer_t * layer ){

	if( stackIndex < STACK_SIZE ){
		stack[stackIndex] = layer;
		stack[stackIndex]->draw = 0;
		stackIndex++;
	}
}

void tlcd_set_layer( Layer_t * layer, int16_t x, int16_t y, const uint16_t * src, int32_t alpha ){

	layer->src = src;

	layer->x = x;
	layer->y = y;
	layer->w = _TLCD_PGM_READ_WORD( layer->src );
	layer->h = _TLCD_PGM_READ_WORD( layer->src + 1 );
	layer->alpha = alpha;

    tlcd_add_layer( layer );
}


void tlcd_draw_layer( Layer_t * layer ){

	Box_t box;
	Box_t win_box;

	box.p1.x = layer->x;
	box.p2.x = layer->x + layer->w;
	box.p3.x = layer->x + layer->w;
	box.p4.x = layer->x;

	box.p1.y = layer->y;
	box.p2.y = layer->y;
	box.p3.y = layer->y + layer->h;
	box.p4.y = layer->y + layer->h;

	if( box.p1.x > tlcd_width || box.p1.y > tlcd_height || box.p3.x < 0 || box.p3.y < 0 ) return;

    if( box.p1.x < 0 ){
    	win_box.p1.x = 0;
    	win_box.p2.x = box.p2.x;
    	win_box.p3.x = box.p3.x;
    	win_box.p4.x = 0;
    }else if( box.p3.x > tlcd_width ){
    	win_box.p1.x = box.p1.x;
    	win_box.p2.x = tlcd_width;
    	win_box.p3.x = tlcd_width;
    	win_box.p4.x = box.p4.x;
    }else{
    	win_box.p1.x = box.p1.x;
    	win_box.p2.x = box.p2.x;
    	win_box.p3.x = box.p3.x;
    	win_box.p4.x = box.p4.x;
    }

    if( box.p1.y < 0 ){
    	win_box.p1.y = 0;
		win_box.p2.y = 0;
		win_box.p3.y = box.p3.y;
		win_box.p4.y = box.p4.y;
    }else if( box.p3.y > tlcd_height ){
    	win_box.p1.y = box.p1.y;
		win_box.p2.y = box.p2.y;
		win_box.p3.y = tlcd_height;
		win_box.p4.y = tlcd_height;
    }else{
    	win_box.p1.y = box.p1.y;
		win_box.p2.y = box.p2.y;
		win_box.p3.y = box.p3.y;
		win_box.p4.y = box.p4.y;
	}

    tlcd_set_addr_win( win_box.p1.x, win_box.p1.y, win_box.p3.x - 1, win_box.p3.y - 1 );

  	_TLCD_SET_RS;
  	_TLCD_CLR_CS;

  	const uint16_t * bitmap = layer->src + 2;

  	layer->draw = 1;

	for( int16_t i=box.p1.y; i<box.p3.y; i++ ){
		for( int16_t j=box.p1.x; j<box.p3.x; j++ ){

			if( (j >= box.p1.x) && (j < box.p3.x) && // if box area
				(i >= box.p1.y) && (i < box.p3.y) ){

				if( (j >= 0) && (j < tlcd_width) && // if box in window area
					(i >= 0) && (i < tlcd_height) ){

					uint16_t color = _TLCD_PGM_READ_WORD( bitmap );
					if( color == layer->alpha ){

						color = bgColor;

						for( uint8_t k=0; k<stackIndex; k++ ){

							if( layer != stack[k] ){

								if( !stack[k]->draw ) continue;

								if( (j >= stack[k]->x) && (j <= (stack[k]->x + stack[k]->w - 1)) &&
									(i >= stack[k]->y) && (i <= (stack[k]->y + stack[k]->h - 1)) ){

									int16_t idx = 2 + abs(j - stack[k]->x) + abs(i - stack[k]->y) * stack[k]->w;
									uint16_t new_color = _TLCD_PGM_READ_WORD( stack[k]->src + idx );

									if( new_color != stack[k]->alpha ) color = new_color;

								}
							}

						}
					}

					tlcd_write_color( color );

				}

				bitmap++;

			}
		}
	}

	_TLCD_SET_CS;
}


void tlcd_draw_bitmap_rle( int16_t x, int16_t y, const uint16_t * bitmap ){

	uint8_t w = _TLCD_PGM_READ_WORD( bitmap++ );
	uint8_t h = _TLCD_PGM_READ_WORD( bitmap++ );
	uint16_t s = _TLCD_PGM_READ_WORD( bitmap++ ) / 2;

	if( x >= tlcd_width || y >= tlcd_height ) return;
	if( (x + w - 1) >= tlcd_width || (y + h - 1) >= tlcd_height ) return;


	tlcd_set_addr_win( x, y, x + w - 1, y + h - 1 );

  	_TLCD_SET_RS;
  	_TLCD_CLR_CS;

	uint16_t rep;
	uint16_t color;

	for( uint16_t i=0; i<s; i++ ){

		rep = _TLCD_PGM_READ_WORD( bitmap++ );
		color = _TLCD_PGM_READ_WORD( bitmap++ );

		for( uint16_t k=0; k<rep; k++ ){
			tlcd_write_color( color );
		}
	}

	_TLCD_SET_CS;
}


