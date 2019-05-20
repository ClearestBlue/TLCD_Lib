/*
 * timers.h
 *
 *  Created on: 20.05.2019
 *      Author: Mariusz
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#define TIMER_IN_USE 2

typedef struct{
	uint8_t flag;
	uint16_t interval;
	uint16_t counter;
	void (*event)( void );
}Timer_t;

extern TIM_HandleTypeDef myMainTimer; 	// TIM4

void setTimer( Timer_t * timer, uint16_t msec, void(*event)( void ) );
void TIMERS_EVENTS( void );

#endif /* TIMERS_H_ */
