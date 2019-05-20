/*
 * periph.c
 *
 *  Created on: 19.05.2019
 *      Author: Mariusz
 */
#include "stm32f7xx.h"
#include "periph.h"

#define SCK_Pin 	GPIO_PIN_5
#define MOSI_Pin 	GPIO_PIN_7
#define TX_Pin		GPIO_PIN_8
#define RX_Pin		GPIO_PIN_9

#define SCK_GPIO_Port 	GPIOA
#define MOSI_GPIO_Port 	GPIOA
#define TX_Port			GPIOD
#define RX_Port			GPIOD

SPI_HandleTypeDef mySpi;
UART_HandleTypeDef myUart;

static void pushInitError( void ){

	while( 1 ){

		HAL_GPIO_WritePin( LED_RED_Port, LED_RED_Pin, GPIO_PIN_SET );
		HAL_Delay( 100 );
		HAL_GPIO_WritePin( LED_RED_Port, LED_RED_Pin, GPIO_PIN_RESET );
		HAL_Delay( 500 );
	}

}

void spiInit( void ){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = SCK_Pin | MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

	mySpi.Instance = SPI1;
	mySpi.Init.Mode = SPI_MODE_MASTER;
	mySpi.Init.Direction = SPI_DIRECTION_1LINE;
	mySpi.Init.DataSize = SPI_DATASIZE_8BIT;
	mySpi.Init.CLKPolarity = SPI_POLARITY_LOW;
	mySpi.Init.CLKPhase = SPI_PHASE_1EDGE;
	mySpi.Init.NSS = SPI_NSS_SOFT;
	mySpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	mySpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	mySpi.Init.TIMode = SPI_TIMODE_DISABLE;
	mySpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	mySpi.Init.CRCPolynomial = 7;
	mySpi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	mySpi.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if( HAL_SPI_Init( &mySpi ) != HAL_OK ){
		pushInitError();
	}

}

void uartInit( void ){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/**USART3 GPIO Configuration
	PD8     ------> USART3_TX
	PD9     ------> USART3_RX
	*/
	GPIO_InitStruct.Pin = TX_Pin | RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );

	myUart.Instance = USART3;
	myUart.Init.BaudRate = 9600;
	myUart.Init.WordLength = UART_WORDLENGTH_8B;
	myUart.Init.StopBits = UART_STOPBITS_1;
	myUart.Init.Parity = UART_PARITY_NONE;
	myUart.Init.Mode = UART_MODE_TX_RX;
	myUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	myUart.Init.OverSampling = UART_OVERSAMPLING_16;
	myUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	myUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if( HAL_UART_Init( &myUart ) != HAL_OK ){
		pushInitError();
	}

}

int __io_putchar( int ch ){
	HAL_UART_Transmit( &myUart, (uint8_t*) &ch, 1, 100 );
	return ch;
}

void gpioInit( void ){

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = LED_GREEN_Pin | LED_BLUE_Pin | LED_RED_Pin;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init( GPIOB, &gpio );

}

void SystemClock_Config( void ){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE3 );

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 144;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK ){
		pushInitError();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
							     RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 ) != HAL_OK ){
		pushInitError();
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
	PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
	if( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK ){
		pushInitError();
	}
}
