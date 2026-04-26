#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>
#include "ball.h"
#include "paddle.h"
#include "brick.h"

extern Ball_t ball;
extern Paddle_t paddle;
extern Brick_t bricks[TOTAL_BRICKS];

typedef enum {
    STATE_RUNNING,
    STATE_LOSS,
	STATE_WIN
} GameState_t;

extern GameState_t current_state;

// Các hàm logic game
uint8_t Check_Ball_Hit_Wall(Ball_t* ball);
void Check_Ball_Hit_Paddle(Ball_t* ball, Paddle_t* paddle);
void Check_Ball_Hit_Bricks(Ball_t* ball, Brick_t bricks[]);
void render(void);
void game_loop(void);

#endif
