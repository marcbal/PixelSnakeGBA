#include <screen.h>

#include <gba_video.h>

void M3Screen::strokeVLine(u8 x, u8 y, u8 length, u16 c) {
    for (u8 cy = y, yEnd = cy + length; cy < yEnd; cy++) {
        setPixel(x, cy, c);
    }
}

void M3Screen::strokeHLine(u8 x, u8 y, u8 length, u16 c) {
    for (u8 cx = x, xEnd = cx + length; cx < xEnd; cx++) {
        setPixel(cx, y, c);
    }
}

void M3Screen::strokeRect(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u16 c) {
    u8 length = xEnd - xStart + 1;
    strokeHLine(xStart, yStart, length, c);
    strokeHLine(xStart, yEnd, length, c);
    
    yStart++; yEnd--; // for vertical line, dont draw corners again

    length = yEnd - yStart + 1;
    strokeVLine(xStart, yStart, length, c);
    strokeVLine(xEnd, yStart, length, c);
}

void M3Screen::fillRect(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u16 c) {
    u8 width = xEnd - xStart + 1;
    for (; yStart <= yEnd; yStart++) {
        strokeHLine(xStart, yStart, width, c);
    }
}
