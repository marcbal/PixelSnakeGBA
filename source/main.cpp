#include <buttons.h>
#include <screen.h>

#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdlib.h> // for random





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

class Snake {
    public:
        Position tail[SNAKE_MAX_SIZE]; // 0 is head
        u16 length;

        void reset() {
            length = 1;
            tail[0].x = ((unsigned int)rand()) % GAME_WIDTH + GAME_X;
            tail[0].y = ((unsigned int)rand()) % GAME_HEIGHT + GAME_Y;
        }
    
        void forward(Position nextPos, bool grow) {
            for (int i = length + 1; i > 0; i--) {
                tail[i] = tail[i-1];
            }
            tail[0] = nextPos;
            if (grow)
                length++;
        }
};


class Game {
    private:
        CellState cells[GAME_X + GAME_WIDTH][GAME_Y + GAME_HEIGHT];
        

        Snake snake;
        Direction currentDirection;
        bool gameEnd;
        bool pause;
        bool fast;
        u16 maxScore = 0;

        u32 tickCount;
        u16 tickMod; // snake step when = 0
        
    public:
        void setCell(Position p, CellState state) {
            cells[p.x][p.y] = state;
            u16 c = (state == TAIL) ? WHITE :
                    (state == FOOD) ? RED :
                    BLACK;
            M3Screen::fillRect(p.x * 8, p.y * 8, (p.x + 1) * 8 - 1, (p.y + 1) * 8 - 1, c);
        }

        inline CellState getCell(Position p) {
            return cells[p.x][p.y];
        }

        void clearBoard() {
            for (int i = 0; i < GAME_X + GAME_WIDTH; i++)
                for (int j = 0; j < GAME_Y + GAME_HEIGHT; j++)
                    cells[i][j] = EMPTY;

            M3Screen::fillRect(GAME_X_MIN, GAME_Y_MIN, GAME_X_MAX - 1, GAME_Y_MAX - 1, BLACK);
        }
        

        void spawnFood() {
            Position food;
            do {
                food.x = ((unsigned int)rand()) % GAME_WIDTH + GAME_X;
                food.y = ((unsigned int)rand()) % GAME_HEIGHT + GAME_Y;
            } while (getCell(food) != EMPTY);
            setCell(food, FOOD);
        }

        

        void finish() {
            gameEnd = true;
            if (snake.length > maxScore) {
                maxScore = snake.length;
            }
        }


        

        void handleInput() {	
            Buttons::update();

            // check input state
            if (Buttons::isHit(KEY_START)) {
                if (gameEnd) { // restart new game
                    init();
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




        void update() {
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
                finish();
                return;
            }

            if (getCell(nextPos) == FOOD) {
                snake.forward(nextPos, true);
                setCell(nextPos, TAIL);
                // spawn new food
                if (snake.length == SNAKE_MAX_SIZE) { // WIN
                    finish();
                    return;
                }
                spawnFood();
            }
            else if (getCell(nextPos) == TAIL) {
                gameEnd = true;
            }
            else { // snake avance normalement
                snake.forward(nextPos, false);
                setCell(nextPos, TAIL);
                Position freePos = snake.tail[snake.length];
                setCell(freePos, EMPTY);
            }


        }




        void init() {
            M3Screen::init();

            // draw borders
            M3Screen::strokeRect(GAME_X_MIN - 2, GAME_Y_MIN - 2, GAME_X_MAX + 1, GAME_Y_MAX + 1, WHITE);
            M3Screen::strokeRect(GAME_X_MIN - 1, GAME_Y_MIN - 1, GAME_X_MAX + 0, GAME_Y_MAX + 0, RED);

            snake.reset();
            clearBoard();
            setCell(snake.tail[0], TAIL);

            spawnFood();

            currentDirection = NONE;
            tickMod = 0;
            gameEnd = false;
            pause = false;
        }

        void tick() {

            handleInput();
            update();

            M3Screen::setPixel(0, 3, gameEnd ? WHITE : GRAY);
            M3Screen::setPixel(2, 3, pause ? WHITE : GRAY);
        }

};

static Game game;




int main() {
    game.init();
    
    irqInit();
    irqEnable(IRQ_VBLANK);

    for(;;) {
        VBlankIntrWait();
        game.tick();
    }
}
