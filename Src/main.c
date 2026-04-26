#include "stm32f4xx.h"
#include "oled.h"
#include "ball.h"
#include "paddle.h"
#include "brick.h"
#include "game_logic.h"
#include "button_ctrl.h"
#include "game_timer.h"

int main(void) {
	// 1. Khởi tạo phần cứng cơ bản

	    i2c_init();
	    dma_init();
	    oled_init();
	    Button_Init();

	    // 2. Khởi tạo các đối tượng Game
	    Ball_Init(&ball, 55, 55);
	    Paddle_Init(&paddle, 52, 60);
	    Level_Init(bricks, 2, 5);

	    // 3. Khởi động các Timer ngắt
	    Timer3_Init(); // Bật đọc nút bấm
	    timer2_init(); // Bật nhịp tim game (game loop)

	    while(1) {
	    }
}
