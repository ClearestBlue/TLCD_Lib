/*
 * timers.c
 *
 *  Created on: 20.05.2019
 *      Author: Mariusz
 */
#include "stm32f7xx.h"
#include "timers.h"

TIM_HandleTypeDef myMainTimer; 	// TIM2

static volatile Timer_t * timers[TIMER_IN_USE];
static volatile uint8_t timerCounter;


void setTimer( Timer_t * timer, uint16_t msec, void(*event)( void ) ){

	if( !timerCounter ){

//  TIMER 4
//	t=1[ms] f=1000[Hz]
//	f_CPU=16[MHz]

		__HAL_RCC_TIM2_CLK_ENABLE();

		myMainTimer.Instance = TIM2;
		myMainTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
		myMainTimer.Init.Prescaler = 7200 - 1;
		myMainTimer.Init.Period = 10 - 1;
		myMainTimer.Init.ClockDivision = 0;
		myMainTimer.Init.AutoReloadPreload = TIM_AUTOMATICOUTPUT_DISABLE;
		myMainTimer.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init( &myMainTimer );
		HAL_TIM_Base_Start_IT( &myMainTimer );
		HAL_NVIC_EnableIRQ( TIM2_IRQn );
	}

	timer->interval = msec;
	timer->counter = timer->interval;
	timer->flag = 0;
	timer->event = event;

	timers[timerCounter] = timer;
	timerCounter++;
}


void TIMERS_EVENTS( void ){

	for( uint8_t i=0; i<timerCounter; i++ ){
		if( timers[i] ){
			if( timers[i]->flag ){
				if( timers[i]->event ) timers[i]->event();
				timers[i]->flag = 0;
			}
		}
	}

}


void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&myMainTimer);
}

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim ){

	for( uint8_t i=0; i<timerCounter; i++ ){
		if( timers[i]->counter ) timers[i]->counter--;
		else{
			timers[i]->counter = timers[i]->interval;
			timers[i]->flag = 1;
		}
	}

}
