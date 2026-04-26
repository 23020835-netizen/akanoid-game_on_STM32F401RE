#include "button_ctrl.h"
#include "stm32f4xx.h"
#include "paddle.h"

extern Paddle_t paddle; // Mượn biến paddle từ game_logic.c
uint8_t speed_divider = 0;


void Timer3_Init(void) {
    //Bật Clock cho TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    //chia thành xung 10kHz => 0.1ms
    TIM3->PSC = 1600 - 1;
    TIM3->ARR = 300 - 1; // 300 * 0.1ms = 30ms ngắt 1 lần

    //gỡ mặt nạ ngắt
    TIM3->DIER |= TIM_DIER_UIE;

    //Bật Timer
    TIM3->CR1 |= TIM_CR1_CEN;

    //Cấu hình NVIC
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,2);
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF; // Xóa cờ ngắt

            // Nếu nút trái đang nhấn
            if ((GPIOA->IDR & (1<<0)) == 0) {
                Paddle_MoveLeft(&paddle);
            }

            // Nếu nút phải đang nhấn
            if ((GPIOA->IDR & (1<<1)) == 0) {
                Paddle_MoveRight(&paddle);
            }
    }
}

void Button_Init(void) {
    //Cấp xung clk cho Port A
    RCC->AHB1ENR |= (1 << 0);

    GPIOA->MODER &= ~((3 << 0) | (3 << 2)); // Cấu hình PA0 và PA1 làm nút

    //cấu hình chế độ pull up
    GPIOA->PUPDR &= ~((3 << 0) | (3 << 2));
    GPIOA->PUPDR |= ((1 << 0) | (1 << 2));
}
