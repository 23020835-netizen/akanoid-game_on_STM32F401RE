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
	    // Đặt trạng thái ban đầu
	    current_state = STATE_START;

	    Timer3_Init(); // Bật quét nút bấm (Timer chạy nền 30ms)
	    timer2_init(); // Khởi tạo nhưng TIM2 chưa đếm

	    // Đẩy khung hình Start ra màn hình lần đầu tiên
	    render();
	    while(1) {
	    }
}
