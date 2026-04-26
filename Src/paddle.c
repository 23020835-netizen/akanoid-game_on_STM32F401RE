#include "paddle.h"

void Paddle_Init(Paddle_t* self, int16_t start_x, int16_t start_y) {
    self->x = start_x;
    self->y = start_y;
    self->w = 24;       // Ván dài 24 pixel
    self->h = 3;        // Ván dày 3 pixel
    self->dx = 4;       // Mỗi lần di chuyển nhảy 4 pixel
}

void Paddle_MoveLeft(Paddle_t* self) {
    self->x -= self->dx;
    // Chặn không cho lọt ra khỏi mép trái
    if (self->x < 0) {
        self->x = 0;
    }
}

void Paddle_MoveRight(Paddle_t* self) {
    self->x += self->dx;
    // Chặn không cho lọt ra khỏi mép phải (128 - chiều rộng ván)
    if (self->x > (128 - self->w)) {
        self->x = 128 - self->w;
    }
}
