/*
 * tlcd_config.c
 *
 *  Created on: 19 maj 2019
 *      Author: Mariusz
 */
#include "TLCD/tlcd_config.h"
#include "periph.h"

void tlcd_gpio_init( void ){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin = CS_Pin | RS_Pin | RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );

}

uint8_t tlcd_pgm_read_byte( const uint8_t * addr ){
	return *addr;
}

uint16_t tlcd_pgm_read_word( const uint16_t * addr ){
	return *addr;
}

void spiTransfer( uint8_t byte ){
	HAL_SPI_Transmit( &mySpi, &byte, 1, 10 );
}


