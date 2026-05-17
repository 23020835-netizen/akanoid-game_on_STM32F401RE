#ifndef PADDLE_H
#define PADDLE_H

#include <stdint.h>

typedef struct {
    int16_t x;      // Tọa độ X góc trên trái
    int16_t y;      // Tọa độ Y góc trên trái
    int16_t w;      // Chiều rộng (Width)
    int16_t h;      // Chiều cao (Height)
    int16_t dx;     // Tốc độ di chuyển mỗi lần nhấn nút
} Paddle_t;

void Paddle_Init(Paddle_t* self, int16_t start_x, int16_t start_y);
void Paddle_MoveLeft(Paddle_t* self);
void Paddle_MoveRight(Paddle_t* self);

#endif /* PADDLE_H */
