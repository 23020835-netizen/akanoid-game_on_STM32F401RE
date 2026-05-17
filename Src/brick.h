#ifndef BRICK_H
#define BRICK_H

#include <stdint.h>

// Định nghĩa số lượng gạch
#define BRICK_ROWS 3
#define BRICK_COLS 8
#define TOTAL_BRICKS (BRICK_ROWS * BRICK_COLS)

typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint8_t active; // 1 = Đang hiển thị, 0 = Đã bị vỡ
} Brick_t;

// Khởi tạo toàn bộ mảng gạch
void Level_Init(Brick_t bricks[], int16_t start_x, int16_t start_y);

#endif
