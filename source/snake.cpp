#include <snake.h>

#include <buttons.h>
#include <screen.h>

#include <stdlib.h>





#define GAME_X_MIN 8
#define GAME_X_MAX SCREEN_WIDTH-8
#define GAME_Y_MIN 8
#define GAME_Y_MAX SCREEN_HEIGHT-8

#define GAME_X 1
#define GAME_Y 1
#define GAME_WIDTH 28
#define GAME_HEIGHT 18
#define SNAKE_MAX_SIZE 504 // 18*28



#define WHITE RGB5(31, 31, 31)
#define RED   RGB5(31, 0, 0)
#define BLACK RGB5(0, 0, 0)
#define GRAY  RGB5(16, 16, 16)



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
	u16 c = (state == TAIL) ? WHITE : (state == FOOD) ? RED : BLACK;
    M3Screen::fillRect(x * 8, y * 8, (x + 1) * 8 - 1, (y + 1) * 8 - 1, c);
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









static void game_input() {	
	Buttons::update();

	// check input state
	if (Buttons::isHit(KEY_START)) {
        if (gameEnd) { // restart new game
		    snake_init();
        }
        else {
            pause = !pause;
        }
	}

    fast = Buttons::isDown(KEY_A);

	if (Buttons::isDown(KEY_LEFT)) {
		currentDirection = LEFT;
	}
	else if (Buttons::isDown(KEY_RIGHT)) {
		currentDirection = RIGHT;
	}
	else if (Buttons::isDown(KEY_UP)) {
		currentDirection = TOP;
	}
	else if (Buttons::isDown(KEY_DOWN)) {
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




void snake_init() {
	M3Screen::init();

	// draw borders
	M3Screen::strokeRect(GAME_X_MIN - 2, GAME_Y_MIN - 2, GAME_X_MAX + 1, GAME_Y_MAX + 1, WHITE);
	M3Screen::strokeRect(GAME_X_MIN - 1, GAME_Y_MIN - 1, GAME_X_MAX + 0, GAME_Y_MAX + 0, RED);

	M3Screen::fillRect(GAME_X_MIN, GAME_Y_MIN, GAME_X_MAX - 1, GAME_Y_MAX - 1, BLACK);


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




void snake_tick() {
	tick++;

	game_input();
	game_update();

    M3Screen::setPixel(0, 3, gameEnd ? WHITE : GRAY);
    M3Screen::setPixel(2, 3, pause ? WHITE : GRAY);

}
