#ifndef BALL_H
#define BALL_H

#include <stdint.h>

// Khai báo cấu trúc
typedef struct {
    int16_t x;
    int16_t y;
    int16_t r;
    int16_t dx;
    int16_t dy;
} Ball_t;

void Ball_Init(Ball_t* self, int16_t start_x, int16_t start_y);
void Ball_Update(Ball_t* self);

#endif
