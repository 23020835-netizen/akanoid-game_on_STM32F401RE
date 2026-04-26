#include "ball.h"

//Constructor
void Ball_Init(Ball_t* self, int16_t start_x, int16_t start_y) {
    self->x = start_x;
    self->y = start_y;
    self->r = 2;
    self->dx = 1;      // Tốc độ ban đầu
    self->dy = -1;
}

// Hàm cập nhật vật lý
void Ball_Update(Ball_t* self) {
    self->x += self->dx;
    self->y += self->dy;
}
