#include <screen.h>

#include <gba_video.h>




u16 Pixel_toM3Index(Pixel p) {
    return p.coord.x + p.coord.y * SCREEN_WIDTH;
}

Pixel Pixel_xy(u8 x, u8 y) {
    return (Pixel) {.coord={x,y}};
}


void screen_m3init() {
    SetMode(MODE_3 | BG2_ENABLE);
}

void screen_m3setMemoryVideo(u16* buffer) {
    for (u16 i = 0, max = SCREEN_WIDTH * SCREEN_HEIGHT; i < max; i++) {
        ((u16*) MODE3_FB)[i] = buffer[i];
    }
}

u16 screen_m3getPixel(Pixel p) {
    return MODE3_FB[p.coord.y][p.coord.x];
}

void screen_m3clear(Color c) {
    for (u16 i = 0, max = SCREEN_WIDTH * SCREEN_HEIGHT; i < max; i++) {
        ((u16*) MODE3_FB)[i] = (u16) c;
    }
}

void screen_m3setPixel(Pixel p, Color c) {
    MODE3_FB[p.coord.y][p.coord.x] = (u16) c;
}

void screen_m3setPixel8(Pixel p8, Color c) {
    screen_m3fillRect(
            Pixel_xy(p8.coord.x * 8, p8.coord.y * 8),
            Pixel_xy((p8.coord.x + 1) * 8 - 1, (p8.coord.y + 1) * 8 - 1),
            c);
}

void screen_m3strokeVLine(Pixel top, u8 length, Color c) {
    for (u16 i = Pixel_toM3Index(top), iEnd = i + length * SCREEN_WIDTH;
            i < iEnd;
            i += SCREEN_WIDTH) {
                ((u16*) MODE3_FB)[i] = (u16) c;
    }
}

void screen_m3strokeHLine(Pixel left, u8 length, Color c) {
    for (u16 i = Pixel_toM3Index(left), iEnd = i + length;
            i < iEnd;
            i++) {
                ((u16*) MODE3_FB)[i] = (u16) c;
    }
}

void screen_m3strokeRect(Pixel topLeft, Pixel bottomRight, Color c) {
    u8 length = bottomRight.coord.x - topLeft.coord.x + 1;
    Pixel tmpCorder = Pixel_xy(topLeft.coord.x, bottomRight.coord.y);
    screen_m3strokeHLine(topLeft, length, c);
    screen_m3strokeHLine(tmpCorder, length, c);
    // vertical line, dont draw corners again
    topLeft.coord.y++;
    bottomRight.coord.y--;
    length = bottomRight.coord.y - topLeft.coord.y + 1;
    tmpCorder = Pixel_xy(bottomRight.coord.x, topLeft.coord.y);
    screen_m3strokeVLine(topLeft, length, c);
    screen_m3strokeVLine(tmpCorder, length, c);
}

void screen_m3fillRect(Pixel topLeft, Pixel bottomRight, Color c) {
    u8 width = bottomRight.coord.x - topLeft.coord.x + 1;
    for (; topLeft.coord.y <= bottomRight.coord.y; topLeft.coord.y++) {
        screen_m3strokeHLine(topLeft, width, c);
    }
}
