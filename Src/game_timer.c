#include "game_timer.h"
#include "stm32f4xx.h"
#include "game_logic.h"

void timer2_init(){
	// bat xung clk cho timer2
	RCC->APB1ENR |= 1;

	TIM2->PSC = 15999; // giam xuong 1kHz
	TIM2->ARR = 32; // 33ms ngat 1 lan

	TIM2->CR1 |= 1; //bat timer
	TIM2->DIER |= 1; //go mat na ngat
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,1);
}


void TIM2_IRQHandler(){
	if( (TIM2->SR & 1) ){
		TIM2->SR= ~1;
		game_loop();
	}
}
