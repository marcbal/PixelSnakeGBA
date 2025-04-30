#include <snake.h>

#include <gba_interrupt.h>
#include <gba_systemcalls.h>


int main() {
	snake_init();
	
	irqInit();
	irqEnable(IRQ_VBLANK);

	for(;;) {
		VBlankIntrWait();
		snake_tick();
	}
}
