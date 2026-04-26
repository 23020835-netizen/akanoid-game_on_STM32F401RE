#include "brick.h"

void Level_Init(Brick_t bricks[], int16_t start_x, int16_t start_y) {
    int16_t b_width = 14;
    int16_t b_height = 6;
    int16_t spacing = 1;   // Khoảng cách giữa các viên

    int index = 0;

    // Vòng lặp quét để tự động xếp gạch thành lưới
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            bricks[index].x = start_x + col * (b_width + spacing);
            bricks[index].y = start_y + row * (b_height + spacing);
            bricks[index].w = b_width;
            bricks[index].h = b_height;
            bricks[index].active = 1;

            index++;
        }
    }
}
