#include "button_ctrl.h"
#include "stm32f4xx.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "game_logic.h"

extern Paddle_t paddle;
extern Ball_t ball;
extern Brick_t bricks[];
extern GameState_t current_state;
extern void render(void);

void Timer3_Init(void) {
    //Bật Clock cho TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    //chia thành xung 10kHz => 0.1ms
    TIM3->PSC = 1600 - 1;
    TIM3->ARR = 300 - 1; // 300 * 0.1ms = 30ms ngắt 1 lần

    //gỡ mặt nạ ngắt
    TIM3->DIER |= TIM_DIER_UIE;

    //Cấu hình NVIC
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,2);
}

void Button_Init(void) {
    //Cấp xung clk cho Port A
    RCC->AHB1ENR |= (1 << 0);

    GPIOA->MODER &= ~((3 << 0) | (3 << 2)); // Cấu hình PA0 và PA1 làm nút điều khiển bóng


    //cấu hình chế độ pull up
    GPIOA->PUPDR &= ~((3 << 0) | (3 << 2));
    GPIOA->PUPDR |= ((1 << 0) | (1 << 2));

    // cấu hình ngắt cho pc3
    RCC->AHB1ENR |= (1 << 2);

    GPIOC->MODER &= ~(3 << 6);

    GPIOC->PUPDR &= ~(3 << 6);
    GPIOC->PUPDR |= (1 << 6);

    RCC->APB2ENR |= (1 << 14);       // Bật Clock SYSCFG
    SYSCFG->EXTICR[0] |= (2 << 12);

    // Chọn sườn xuống
    EXTI->FTSR |= (1 << 3);

    // Gỡ mặt nạ ngắt
    EXTI->IMR |= (1 << 3);

    // Cấu hình NVIC
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 1);
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & 1) {
        TIM3->SR = ~1; // Xóa cờ ngắt

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

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1 << 3)) {

        // 1. Nếu đang ở màn hình Start-> Chuyển sang Chơi
        if (current_state == STATE_START) {
            current_state = STATE_RUNNING;
            // Reset lại tọa độ vật lý, kích thước và trạng thái của các thực thể
            Ball_Init(&ball, 55, 55);
            Paddle_Init(&paddle, 52, 60);
            Level_Init(bricks, 2, 5);

            TIM2->CR1 |= 1;  // Bật Game Loop (TIM2 bắt đầu đếm)
            TIM3->CR1 |= TIM_CR1_CEN;
        }

        // 2. Nếu đang Chơi -> Tạm dừng Game
        else if (current_state == STATE_RUNNING) {
            current_state = STATE_PAUSED;
            TIM2->CR1 &= ~1; // Tắt 2 bộ timer để tiết kiệm điện năng
            TIM3->CR1 &= ~1;
            render();
        }

        // 3. Nếu đã kết thúc Game (WIN hoặc LOSS) -> Khởi tạo lại và về màn hình Start
        else if (current_state == STATE_WIN || current_state == STATE_LOSS) {
            current_state = STATE_START; // Đưa trạng thái về ban đầu
            render();
        }
        // 4. Nếu đang tạm dừng
        else if(current_state == STATE_PAUSED) {
            current_state = STATE_RUNNING;
            TIM2->CR1 |= 1;  // Bật Game Loop (TIM2 bắt đầu đếm)
            TIM3->CR1 |= TIM_CR1_CEN;
        }

        // CHỐNG DỘI PHÍM
        for(volatile uint32_t i = 0; i < 30000; i++);

        // xóa cờ ngắt
        EXTI->PR = (1 << 3);
    }
}
