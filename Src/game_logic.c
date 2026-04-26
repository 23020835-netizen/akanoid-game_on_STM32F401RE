#include "game_logic.h"
#include "oled.h"
#include <string.h>

Ball_t ball;
Paddle_t paddle;
Brick_t bricks[TOTAL_BRICKS];
GameState_t current_state;

void render() {
    memset(buffer, 0x00, 1024); // Xóa trắng buffer

    if(current_state == STATE_LOSS){
    	Draw_LOSS(50,30);
    	xuat_data();
    	return;
    }
    if(current_state == STATE_WIN){
    	Draw_WIN(50,30);
    	xuat_data();
    	return;
    }

    // 1. Vẽ quả bóng
    DrawBall(ball.x, ball.y, ball.r);

    // 2. Vẽ Ván đỡ (Dùng hình chữ nhật đặc)
    FillRect(paddle.x, paddle.y, paddle.w, paddle.h);

    // 3. Vẽ mảng Gạch
    for (int i = 0; i < TOTAL_BRICKS; i++) {
        // Chỉ vẽ những viên gạch CÒN SỐNG (active == 1)
        if (bricks[i].active == 1) {
            // Vẽ viền ngoài cho viên gạch
            DrawRect(bricks[i].x, bricks[i].y, bricks[i].w, bricks[i].h);
        }
    }

    // Đẩy dữ liệu ra OLED
    xuat_data();
}

// Trả về 1 nếu bóng rơi xuống đáy, 0 nếu bình thường
uint8_t Check_Ball_Hit_Wall(Ball_t* ball) {
    uint8_t ball_dropped = 0;

    // 1. Tường Trái
    if (ball->x <= ball->r) {
        ball->x = ball->r;
        ball->dx = -ball->dx;
    }
    // 2. Tường Phải (Biên 127)
    else if (ball->x >= (128 - ball->r - 1)) {
        ball->x = 128 - ball->r - 1;
        ball->dx = -ball->dx;
    }

    // 3. Trần nhà (Biên 0)
    if (ball->y <= ball->r) {
        ball->y = ball->r;
        ball->dy = -ball->dy;
    }
    // 4. Sàn nhà (Biên 63) - Rớt bóng!
    else if (ball->y >= (64 - ball->r - 1)) {
        ball->y = 64 - ball->r - 1;
        ball_dropped = 1;     // Phất cờ báo hiệu đã rơi
    }

    return ball_dropped;
}

void Check_Ball_Hit_Paddle(Ball_t* ball, Paddle_t* paddle) {
    // 4 cạnh hộp bao quanh quả bóng
    int16_t b_left   = ball->x - ball->r;
    int16_t b_right  = ball->x + ball->r;
    int16_t b_top    = ball->y - ball->r;
    int16_t b_bottom = ball->y + ball->r;

    // 4 cạnh hộp của ván đỡ
    int16_t p_left   = paddle->x;
    int16_t p_right  = paddle->x + paddle->w;
    int16_t p_top    = paddle->y;
    int16_t p_bottom = paddle->y + paddle->h;

    // Kiểm tra giao nhau AABB
    if (b_right >= p_left && b_left <= p_right &&
        b_bottom >= p_top && b_top <= p_bottom) {

        // Đảo chiều Y (Nảy lên)
        ball->dy = -ball->dy;

        // đẩy bóng ra khỏi ván
        ball->y = p_top - ball->r;
    }
}

void Check_Ball_Hit_Bricks(Ball_t* ball, Brick_t bricks[]) {
    int16_t b_left   = ball->x - ball->r;
    int16_t b_right  = ball->x + ball->r;
    int16_t b_top    = ball->y - ball->r;
    int16_t b_bottom = ball->y + ball->r;

    for (int i = 0; i < TOTAL_BRICKS; i++) {
        // Chỉ xét những viên gạch còn hiện trên màn hình
        if (bricks[i].active == 1) {
            int16_t br_left   = bricks[i].x;
            int16_t br_right  = bricks[i].x + bricks[i].w;
            int16_t br_top    = bricks[i].y;
            int16_t br_bottom = bricks[i].y + bricks[i].h;

            // Kiểm tra va chạm AABB
            if (b_right >= br_left && b_left <= br_right &&
                b_bottom >= br_top && b_top <= br_bottom) {

                // 1. Phá vỡ gạch
                bricks[i].active = 0;

                // 2. Xử lý nảy bóng (Phân biệt đập hông hay đập nắp)
                // Nếu tâm quả bóng nằm giữa cạnh trái/phải của viên gạch -> Đập nắp/đáy
                if (ball->x > br_left && ball->x < br_right) {
                    ball->dy = -ball->dy;
                }
                // Ngược lại -> Đập vào hông (cạnh bên)
                else {
                    ball->dx = -ball->dx;
                }

                // 3. Thoát vòng lặp ngay để mỗi frame chỉ đập 1 viên (Chống bug xuyên gạch)
                break;
            }
        }
    }
}

uint8_t Check_All_Bricks_Broken(Brick_t bricks[]) {
    for (int i = 0; i < TOTAL_BRICKS; i++) {
        if (bricks[i].active == 1) {
            return 0; // Phát hiện còn ít nhất 1 viên gạch sống -> Chưa thắng
        }
    }
    return 1; // Đã duyệt hết mảng mà không thấy viên nào active -> Đã phá hết (Thắng)
}

void game_loop(){
    // 1. Dừng tính toán vật lý nếu game đã kết thúc (Thua hoặc Thắng)
    if (current_state == STATE_LOSS || current_state == STATE_WIN) {
        return;
    }

    // 2. Cập nhật vị trí bóng
    Ball_Update(&ball);

    // 3. Xử lý va chạm tường & kiểm tra rơi bóng
    if (Check_Ball_Hit_Wall(&ball) == 1) {
        current_state = STATE_LOSS; // Rơi xuống đáy -> Thua
    }

    // 4. Xử lý va chạm ván và gạch
    Check_Ball_Hit_Paddle(&ball, &paddle);
    Check_Ball_Hit_Bricks(&ball, bricks);

    // 5. Kiểm tra điều kiện thắng (Phá hết gạch) sau khi đã xét va chạm
    if (Check_All_Bricks_Broken(bricks) == 1) {
        current_state = STATE_WIN; // Chuyển sang trạng thái Thắng
    }

    // 6. Vẽ lại khung hình mới
    render();
}
