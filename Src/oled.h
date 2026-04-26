#ifndef OLED_H
#define OLED_H

#include "stm32f4xx.h"
#include <stdint.h>

// Extern biến mảng để file khác (nếu cần) có thể nhìn thấy
extern uint8_t buffer[1024];

// Các hàm khởi tạo
void i2c_init(void);
void dma_init(void);
void gui_lenh(uint8_t lenh);
void oled_init(void);
void xuat_data(void); // đẩy từ buffer ra màn oled

// Các hàm vẽ đồ họa
void DrawPixel(int16_t x, int16_t y);
void DrawFastVLine(int16_t x, int16_t y, int16_t h);
void DrawFastHLine(int16_t x, int16_t y, int16_t w);
void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h); //hình chữ nhật rỗng
void FillRect(int16_t x, int16_t y, int16_t w, int16_t h); //hình chữ nhật đặc
void DrawBall(int16_t x0, int16_t y0, int16_t r);
void Draw_LOSS(int16_t x, int16_t y);
void Draw_WIN(int16_t x, int16_t y);

#endif /* OLED_H */
