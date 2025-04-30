
#ifndef SCREEN_H
#define SCREEN_H


#include <gba_video.h>


class M3Screen {
    public:
        /// @brief Initialise video mode 3 (bitmap, 240x160px, 16bpp).
        static inline void init() {
            SetMode(MODE_3 | BG2_ENABLE);
        }

        /// @brief Set the color of one pixel.
        /// @param x the x coordinate.
        /// @param y the y coordinate.
        /// @param c the 16 bit color.
        static inline void setPixel(u8 x, u8 y, u16 c) {
            MODE3_FB[y][x] = c;
        }

        /// @brief Stroke a vertical line.
        /// @param x the x coordinate of the starting point.
        /// @param y the y coordinate of the starting point.
        /// @param length the length of the line.
        /// @param c the 16 bit color.
        static void strokeVLine(u8 x, u8 y, u8 length, u16 c);

        /// @brief Stroke a horizontal line.
        /// @param x the x coordinate of the starting point.
        /// @param y the y coordinate of the starting point.
        /// @param length the length of the line.
        /// @param c the 16 bit color.
        static void strokeHLine(u8 x, u8 y, u8 length, u16 c);

        /// @brief Stroke a hollow rectangle.
        /// @param xStart the x coordinate of the top-left point.
        /// @param yStart the y coordinate of the top-left point.
        /// @param xEnd the x coordinate of the bottom-right point.
        /// @param yEnd the y coordinate of the bottom-right point.
        /// @param c the 16 bit color.
        static void strokeRect(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u16 c);

        /// @brief Draw a filled rectangle.
        /// @param xStart the x coordinate of the top-left point.
        /// @param yStart the y coordinate of the top-left point.
        /// @param xEnd the x coordinate of the bottom-right point.
        /// @param yEnd the y coordinate of the bottom-right point.
        /// @param c the 16 bit color.
        static void fillRect(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u16 c);
};






#endif
