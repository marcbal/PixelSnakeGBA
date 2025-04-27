#include <screen.h>
#include <buttons.h>
#include <snake.h>


u32 frame = 0; // runs at 60 Hz (around 16.67 ms)

int main() {
	screen_m3init();
	snake_init();

	for(;;frame++) {
		vsync();
		Button_update();

		// key state
		for (u16 i = 0; i < 10; i++) {
			screen_m3setPixel(Pixel_xy(1 + i, 1), Button_getState(1 << i) ? WHITE : GRAY);
		}

		// tick count
		for (u8 i = 0; i < 32; i++) {
			screen_m3setPixel(Pixel_xy(1 + i, 2), (frame >> i & 1) ? WHITE : GRAY);
		}

		snake_tick(frame);
	}
}
