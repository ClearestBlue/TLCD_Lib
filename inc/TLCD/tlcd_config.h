/*
 * tlcd_config.h
 *
 *  Created on: 19 maj 2019
 *      Author: Mariusz
 */

#ifndef TLCD_TLCD_CONFIG_H_
#define TLCD_TLCD_CONFIG_H_

#include "stm32f7xx.h"
// PD6 - rs
// PD5 - rst

//SPI
// PD7 - CS
// PB3 - MOSI
// PB5 - SCK

#define CS_Port		GPIOD
#define RS_Port		GPIOD
#define RST_Port	GPIOD

#define CS_Pin		GPIO_PIN_7
#define RS_Pin		GPIO_PIN_6
#define RST_Pin		GPIO_PIN_5


void tlcd_gpio_init( void );
uint8_t tlcd_pgm_read_byte( const uint8_t * addr );
uint16_t tlcd_pgm_read_word( const uint16_t * addr );
void spiTransfer( uint8_t byte );

#define _TLCD_DELAY_MS( _MS ) 				HAL_Delay( _MS )
#define _TLCD_SPI_TRANSFER( _DATA ) 		spiTransfer( _DATA )
#define _TLCD_GPIO_INIT()					tlcd_gpio_init()
#define _TLCD_SET_CS						HAL_GPIO_WritePin( CS_Port, CS_Pin, GPIO_PIN_SET )
#define _TLCD_CLR_CS						HAL_GPIO_WritePin( CS_Port, CS_Pin, GPIO_PIN_RESET )
#define _TLCD_SET_RS						HAL_GPIO_WritePin( RS_Port, RS_Pin, GPIO_PIN_SET )
#define _TLCD_CLR_RS						HAL_GPIO_WritePin( RS_Port, RS_Pin, GPIO_PIN_RESET )
#define _TLCD_SET_RST						HAL_GPIO_WritePin( RST_Port, RST_Pin, GPIO_PIN_SET )
#define _TLCD_CLR_RST						HAL_GPIO_WritePin( RST_Port, RST_Pin, GPIO_PIN_RESET )

#define _TLCD_PGM_U8_TYPE					const uint8_t
#define _TLCD_PGM_READ_BYTE( _ADDR )		tlcd_pgm_read_byte( _ADDR )
#define _TLCD_PGM_READ_WORD( _ADDR )		tlcd_pgm_read_word( _ADDR )


#endif /* TLCD_TLCD_CONFIG_H_ */
