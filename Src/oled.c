#include "oled.h"
#define OLED_ADDR 0x78

uint8_t buffer[1024] = {0};

void i2c_init(){
	RCC->AHB1ENR |= 1<<1; //bat portB
	//AF cho pb8 va pb9
	GPIOB->MODER &= ~(3<<16);
	GPIOB->MODER |= (2<<16);
	GPIOB->MODER &= ~(3<<18);
	GPIOB->MODER |= (2<<18);

	// Cấu hình Open-Drain (OTYPER) cho PB8 và PB9
	GPIOB->OTYPER |= (1<<8) | (1<<9);

	// cau hinh af4 cho pb8 va pb9 theo bang af mapping
	GPIOB->AFR[1] |= 4;
	GPIOB->AFR[1] |= (4<<4);

	// bat clk cho i2c_1
	RCC->APB1ENR |= (1<<21);

	// khai bao tan so cap cho khoi i2c
	I2C1->CR2 |=16;
	I2C1->CR2 |= (1<<11); // cau hinh dma lam viec thay


	//bat che do fast mode
	I2C1->CCR |= (1<<15);

	// cau hinh tan so truyen
	I2C1->CCR |= 14;

	I2C1->TRISE = 5; // 16*0.3+1

	I2C1->CR1 |= 1; // bat i2c

}

void dma_init(){
	RCC->AHB1ENR |= (1<<21); // cap clk cho dma1
	DMA1_Stream7->PAR = (uint32_t)&(I2C1->DR); // dia chi ngoai vi
	DMA1_Stream7->M0AR = (uint32_t)&buffer; // dia chi ram
	DMA1_Stream7->NDTR = 1024; //1024 byte

	//mac dinh truyen 1byte
	DMA1_Stream7->CR |= (1<<4); // cho phep ngat tu dma
	DMA1_Stream7->CR |= (1<<6); // ram sang ngoai vi
	DMA1_Stream7->CR |= (1<<10); // ram tu dong tang dia chi
	DMA1_Stream7->CR |= (1<<25); // chon chanel 1 cho stream7

	// mac dinh khong dung fifo

	NVIC_EnableIRQ(DMA1_Stream7_IRQn);
	NVIC_SetPriority(DMA1_Stream7_IRQn,0);
}

void gui_lenh(uint8_t lenh){
	while( (I2C1->SR2 >> 1) & 1); //co busy bang 1 thi bi ket

	I2C1->CR1 |= (1<<8); // dung co start
	while( (I2C1->SR1 & 1) == 0 ); // doi co SB len 1

	I2C1->DR= OLED_ADDR;
	while( ((I2C1->SR1>>1) & 1) == 0); //doi co ADDR len 1
	(void) I2C1->SR2; // phai doc SR2 de ha co xuong

	I2C1->DR= 0x00;
	while( ((I2C1->SR1>>7) & 1) == 0 ); // doi co txe len 1

	I2C1->DR= lenh;
	while( ((I2C1->SR1>>2) & 1) == 0 ); // doi co btf len 1

	I2C1->CR1 |= (1<<9); // dung co stop
}

void oled_init(){
    gui_lenh(0xAE); // Tắt màn hình

    // Cấu hình khung hình vật lý
    gui_lenh(0xA8); gui_lenh(0x3F); // Multiplex Ratio: 64 dòng (BẮT BUỘC)
    gui_lenh(0xD3); gui_lenh(0x00); // Display Offset: 0 (Sửa lỗi lệch dòng)
    gui_lenh(0x40);                 // Display Start Line: 0 (Ghim RAM dòng 0 vào đỉnh màn hình)

    // Cấu hình cách quét dòng
    gui_lenh(0xA1); // Segment Remap
    gui_lenh(0xC8); // COM Output Scan Direction
    gui_lenh(0xDA); gui_lenh(0x12); // COM Pins hardware: 0x12 cho màn 64 dòng

    // Chế độ bộ nhớ
    gui_lenh(0x20); gui_lenh(0x00); // Horizontal Addressing Mode

    // Điện áp và độ sáng
    gui_lenh(0x8D); gui_lenh(0x14); // Bật Charge Pump
    gui_lenh(0xAF); // Bật màn hình
}

void xuat_data(){
	// 1. Reset nơi bắt đầu dữ liệu về (0,0) và đặt giới hạn tăng địa chỉ đến đâu thì quay về
	    gui_lenh(0x21); gui_lenh(0); gui_lenh(127);
	    gui_lenh(0x22); gui_lenh(0); gui_lenh(7);

		while( (I2C1->SR2 >> 1) & 1); //co busy bang 1 thi bi ket

		I2C1->CR1 |= (1<<8); // dung co start
		while( (I2C1->SR1 & 1) == 0 ); // doi co SB len 1

		I2C1->DR= OLED_ADDR;
		while( ((I2C1->SR1>>1) & 1) == 0); //doi co ADDR len 1
		(void) I2C1->SR2; // phai doc SR2 de ha co xuong

		I2C1->DR= 0x40;

		DMA1_Stream7->NDTR = 1024; //1024 byte
		DMA1_Stream7->CR |= (1<<0); // bat dma
}

void DMA1_Stream7_IRQHandler(){
	if( (DMA1->HISR >> 27 & 1) ){
		DMA1->HIFCR = (1<<27); //xoa co ngat

		while( ((I2C1->SR1>>2) & 1) == 0 ); // doi co btf len 1
		I2C1->CR1 |= (1<<9); // dung co stop

		DMA1_Stream7->CR &= ~1; // tat dma
	}
}






void DrawPixel(int16_t x, int16_t y) {
    // Chỉ vẽ nếu nằm trong màn hình
    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
        buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    }
}

void DrawFastVLine(int16_t x, int16_t y, int16_t h) {
    for (int16_t i = y; i < y + h; i++) {
        DrawPixel(x, i);
    }
}


void DrawBall(int16_t x0, int16_t y0, int16_t r) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    // Cột giữa
    DrawFastVLine(x0, y0 - r, 2 * r + 1);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        // Các cột 2 bên
        DrawFastVLine(x0 + x, y0 - y, 2 * y + 1);
        DrawFastVLine(x0 - x, y0 - y, 2 * y + 1);
        DrawFastVLine(x0 + y, y0 - x, 2 * x + 1);
        DrawFastVLine(x0 - y, y0 - x, 2 * x + 1);
    }
}


void DrawFastHLine(int16_t x, int16_t y, int16_t w) {
    for (int16_t i = x; i < x + w; i++) {
        DrawPixel(i, y);
    }
}


void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    DrawFastHLine(x, y, w);                 // Cạnh trên
    DrawFastHLine(x, y + h - 1, w);         // Cạnh dưới
    DrawFastVLine(x, y, h);                 // Cạnh trái
    DrawFastVLine(x + w - 1, y, h);         // Cạnh phải
}


void FillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Quét từ trái sang phải, vẽ các đường dọc sát nhau
    for (int16_t i = x; i < x + w; i++) {
        DrawFastVLine(i, y, h);
    }
}

void Draw_LOSS(int16_t x, int16_t y) {
    // 1. Chữ L
    DrawFastVLine(x, y, 7);             // Nét dọc
    DrawFastHLine(x, y + 6, 5);         // Nét ngang đáy

    // 2. Chữ O (Cách chữ L 2 pixel -> x + 7)
    DrawRect(x + 7, y, 5, 7);           // Vẽ luôn 1 cái hộp rỗng là ra chữ O

    // 3. Chữ S thứ nhất (Cách chữ O 2 pixel -> x + 14)
    DrawFastHLine(x + 14, y, 5);        // Nét ngang trên cùng
    DrawFastVLine(x + 14, y, 3);        // Nét dọc trái (nửa trên)
    DrawFastHLine(x + 14, y + 3, 5);    // Nét ngang giữa
    DrawFastVLine(x + 18, y + 3, 4);    // Nét dọc phải (nửa dưới)
    DrawFastHLine(x + 14, y + 6, 5);    // Nét ngang dưới cùng

    // 4. Chữ S thứ hai (Cách chữ S đầu 2 pixel -> x + 21)
    DrawFastHLine(x + 21, y, 5);
    DrawFastVLine(x + 21, y, 3);
    DrawFastHLine(x + 21, y + 3, 5);
    DrawFastVLine(x + 25, y + 3, 4);
    DrawFastHLine(x + 21, y + 6, 5);
}

void Draw_WIN(int16_t x, int16_t y) {
    // 1. Chữ W (Ghép từ các đường dọc và điểm ảnh)
    DrawFastVLine(x, y, 6);             // Trụ trái
    DrawPixel(x + 1, y + 6);            // Điểm nối đáy trái
    DrawFastVLine(x + 2, y + 3, 3);     // Trụ giữa (thấp hơn)
    DrawPixel(x + 3, y + 6);            // Điểm nối đáy phải
    DrawFastVLine(x + 4, y, 6);         // Trụ phải

    // 2. Chữ I (Cách chữ W 2 pixel -> x + 7)
    DrawFastVLine(x + 7, y, 7);

    // 3. Chữ N (Cách chữ I 2 pixel -> x + 10)
    // Vì bác chưa có hàm vẽ đường chéo (DrawLine), ta dùng DrawPixel để tạo nét chéo
    DrawFastVLine(x + 10, y, 7);        // Trụ trái
    DrawPixel(x + 11, y + 2);           // Nét chéo 1
    DrawPixel(x + 12, y + 3);           // Nét chéo 2
    DrawPixel(x + 13, y + 4);           // Nét chéo 3
    DrawFastVLine(x + 14, y, 7);        // Trụ phải
}
