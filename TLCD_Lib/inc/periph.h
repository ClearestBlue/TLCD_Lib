/*
 * periph.h
 *
 *  Created on: 19.05.2019
 *      Author: Mariusz
 */

#ifndef PERIPH_H_
#define PERIPH_H_

#define LED_GREEN_Port	GPIOB
#define LED_BLUE_Port	GPIOB
#define LED_RED_Port	GPIOB

#define LED_GREEN_Pin	GPIO_PIN_0
#define LED_BLUE_Pin	GPIO_PIN_7
#define LED_RED_Pin		GPIO_PIN_14

extern SPI_HandleTypeDef mySpi;

void spiInit( void );
void uartInit( void );
void gpioInit( void );
void SystemClock_Config( void );

#endif /* PERIPH_H_ */
