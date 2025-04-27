
#ifndef SCREEN_H
#define SCREEN_H


#include <gba_video.h>

/*
    Colors
*/

typedef u16 Color;

#define vsync() while(REG_VCOUNT != 160);

#define WHITE RGB5(31, 31, 31)
#define RED   RGB5(31, 0, 0)
#define BLACK RGB5(0, 0, 0)
#define GRAY RGB5(16, 16, 16)

/*
    Pixels
*/

typedef union {
    u16 wrapped;
    struct {
        u8 x; u8 y;
    } coord;
} Pixel;

Pixel Pixel_xy(u8 x, u8 y);

u16 Pixel_toM3Index(Pixel p);




void screen_m3init();
void screen_m3setMemoryVideo(u16* buffer);
u16  screen_m3getPixel(Pixel p);
void screen_m3clear(Color c);
void screen_m3setPixel(Pixel p, Color c);
void screen_m3setPixel8(Pixel p8, Color c);
void screen_m3strokeVLine(Pixel start, u8 length, Color c);
void screen_m3strokeHLine(Pixel start, u8 length, Color c);
void screen_m3strokeRect(Pixel start, Pixel end, Color c);
void screen_m3fillRect(Pixel start, Pixel end, Color c);




#endif
