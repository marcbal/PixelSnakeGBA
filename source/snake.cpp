#include <snake.h>

#include <buttons.h>
#include <screen.h>

#include <stdlib.h>

typedef struct {
	u8 x;
	u8 y;
} Position;

typedef enum {
	EMPTY, FOOD, TAIL
} CellState;
typedef enum {
	NONE, TOP, RIGHT, BOTTOM, LEFT
} Direction;

typedef struct {
	Position tail[SNAKE_MAX_SIZE]; // 0 is head
	u16 length;
} Snake;


static void Snake_shift(Snake* snake, Position newValue) {
	for (int i = snake->length + 1; i > 0; i--) {
		snake->tail[i] = snake->tail[i-1];
	}
	snake->tail[0] = newValue;
}

static CellState cells[GAME_X + GAME_WIDTH][GAME_Y + GAME_HEIGHT];
static Snake snake;
static Direction currentDirection;
static bool gameEnd;
static bool pause;
static bool fast;
static u16 maxScore = 0;


static u32 tick;
static u16 tickMod; // snake step when = 0



void snake_setCell(u8 x, u8 y, CellState state) {
    cells[x][y] = state;
    screen_m3setPixel8(Pixel_xy(x, y), (state == TAIL) ? WHITE : (state == FOOD) ? RED : BLACK);
}





void game_spawn_food() {
    Position food;
    do {
        food.x = ((unsigned int)rand()) % GAME_WIDTH + GAME_X;
        food.y = ((unsigned int)rand()) % GAME_HEIGHT + GAME_Y;
    } while (cells[food.x][food.y] != EMPTY);
    snake_setCell(food.x, food.y, FOOD);
}


void game_finish() {
    gameEnd = true;
    if (snake.length > maxScore) {
        maxScore = snake.length;
    }
}



void snake_init() {

	// draw borders
	screen_m3strokeRect(Pixel_xy(GAME_X_MIN - 2, GAME_Y_MIN - 2), Pixel_xy(GAME_X_MAX + 1, GAME_Y_MAX + 1), WHITE);
	screen_m3strokeRect(Pixel_xy(GAME_X_MIN - 1, GAME_Y_MIN - 1), Pixel_xy(GAME_X_MAX + 0, GAME_Y_MAX + 0), RED);

	screen_m3fillRect(Pixel_xy(GAME_X_MIN, GAME_Y_MIN), Pixel_xy(GAME_X_MAX - 1, GAME_Y_MAX - 1), BLACK);


	for (int i = 0; i < GAME_X + GAME_WIDTH; i++)
		for (int j = 0; j < GAME_Y + GAME_HEIGHT; j++)
			cells[i][j] = EMPTY;

	snake.length = 1;
	snake.tail[0].x = ((unsigned int)rand()) % GAME_WIDTH + GAME_X;
	snake.tail[0].y = ((unsigned int)rand()) % GAME_HEIGHT + GAME_Y;

	game_spawn_food();

	currentDirection = NONE;

    snake_setCell(snake.tail[0].x, snake.tail[0].y, TAIL);

	tickMod = 0;

	gameEnd = false;

    pause = false;

    fast = false;
}






static void game_input() {
	// check input state
	if (Button_getDown(KEY_START)) { // restart new game
        if (gameEnd) {
		    snake_init();
        }
        else {
            pause = !pause;
        }
	}

    fast = Button_getState(KEY_A);

	if (Button_getState(KEY_LEFT)) {
		currentDirection = LEFT;
	}
	else if (Button_getState(KEY_RIGHT)) {
		currentDirection = RIGHT;
	}
	else if (Button_getState(KEY_UP)) {
		currentDirection = TOP;
	}
	else if (Button_getState(KEY_DOWN)) {
		currentDirection = BOTTOM;
	}
}




static void game_update() {
	if (gameEnd || pause)
		return;
	// update game state
	if (currentDirection == NONE)
		return;
	tickMod++;
	tickMod %= fast ? 6 : 30; // 0.1 or 0.5 s

	if (tickMod != 0)
		return;

	// UPDATE game state
	Position nextPos = snake.tail[0];
	if (currentDirection == LEFT)
		nextPos.x--;
	else if (currentDirection == RIGHT)
		nextPos.x++;
	else if (currentDirection == TOP)
		nextPos.y--;
	else if (currentDirection == BOTTOM)
		nextPos.y++;

	if (nextPos.x < GAME_X ||nextPos.x >= GAME_X + GAME_WIDTH
			|| nextPos.y < GAME_Y || nextPos.y >= GAME_Y + GAME_HEIGHT) {
				// snake sors de l'écran
		game_finish();
		return;
	}

	if (cells[nextPos.x][nextPos.y] == FOOD) {
		Snake_shift(&snake, nextPos);
		snake.length++;
        snake_setCell(nextPos.x, nextPos.y, TAIL);
		// spawn new food
		if (snake.length == SNAKE_MAX_SIZE) { // WIN
			game_finish();
			return;
		}
		game_spawn_food();
	}
	else if (cells[nextPos.x][nextPos.y] == TAIL) {
		gameEnd = true;
	}
	else { // snake avance normalement
		Snake_shift(&snake, nextPos);
        snake_setCell(nextPos.x, nextPos.y, TAIL);
		Position freePos = snake.tail[snake.length];
        snake_setCell(freePos.x, freePos.y, EMPTY);
	}


}





void snake_tick(u32 t) {
	tick = t;





	game_input();
	game_update();

    screen_m3setPixel(Pixel_xy(0, 3), gameEnd ? WHITE : GRAY);
    screen_m3setPixel(Pixel_xy(2, 3), pause ? WHITE : GRAY);
    screen_m3setPixel(Pixel_xy(4, 3), fast ? WHITE : GRAY);

	// tickMod count
	for (u8 i = 0; i < 5; i++) {
		screen_m3setPixel(Pixel_xy(6 + i, 3), (tickMod >> i & 1) ? WHITE : GRAY);
	}


	// max score
	for (u8 i = 0; i < 16; i++) {
		screen_m3setPixel(Pixel_xy(0 + i, 4), (maxScore >> i & 1) ? WHITE : GRAY);
	}
	// current score
	for (u8 i = 0; i < 16; i++) {
		screen_m3setPixel(Pixel_xy(0 + i, 5), (snake.length >> i & 1) ? WHITE : GRAY);
	}
}
