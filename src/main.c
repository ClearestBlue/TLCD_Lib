/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f7xx.h"
#include "periph.h"
#include "TLCD/Driver/tlcd.h"
#include "TLCD/Driver/tlcd_font.h"
#include "timers.h"
#include "img.h"

Layer_t layer2;
Counter_t counter;

void timer0_event( void );
void timer1_event( void );

int main( void ){

	HAL_Init();

	SystemClock_Config();
	gpioInit();
	spiInit();
	uartInit();

    tlcd_init();
    tlcd_set_orientation( TLCD_PORTRAIT_INV );
    tlcd_clear( TLCD_COLOR_CYAN );

    Timer_t Timer0, Timer1;
    setTimer( &Timer0, 50, timer0_event );
    setTimer( &Timer1, 500, timer1_event );

    Layer_t layer1, layer3;
    tlcd_set_layer( &layer1, 0, 0, test, TLCD_NO_ALPHA );
    tlcd_set_layer( &layer2, 30, 30, l3, TLCD_COLOR_WHITE );
    tlcd_set_layer( &layer3, 40, 30, l2, TLCD_COLOR_WHITE );


    counter.x = 80;
    counter.y = 10;
    counter.digits = 4;
    counter.alignment = TLCD_TO_RIGHT;


    tlcd_draw_layer( &layer1 );
//    tlcd_draw_layer( &layer2 );
    tlcd_draw_layer( &layer3 );

    tlcd_draw_text( 60, 110, "Catta", TLCD_COLOR_RED, TLCD_TO_LEFT );


	while(1){

		TIMERS_EVENTS();

	}
}

void timer0_event( void ){

	tlcd_draw_layer( &layer2 );
	tlcd_draw_counter( &counter, TLCD_COLOR_BLUE );

	layer2.x++;
	if( layer2.x > 130 ) layer2.x = 30;

	counter.value++;


}

void timer1_event( void ){

	HAL_GPIO_TogglePin( LED_BLUE_Port, LED_BLUE_Pin );

}
