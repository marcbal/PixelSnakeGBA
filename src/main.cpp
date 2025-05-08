#include <bn_core.h>
#include <bn_keypad.h>
#include <bn_random.h>



#include "bn_regular_bg_tiles_ptr.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"


#include "bn_regular_bg_tiles_items_bg.h"




#define GAME_X 1
#define GAME_Y 2
#define GAME_WIDTH 28
#define GAME_HEIGHT 17
#define SNAKE_MAX_SIZE GAME_HEIGHT*GAME_WIDTH



#define BG_TILE_OUTSIDE_TOP_LEFT      0
#define BG_TILE_OUTSIDE_TOP           1
#define BG_TILE_OUTSIDE_TOP_RIGHT     2
#define BG_TILE_OUTSIDE_LEFT         16
#define BG_TILE_OUTSIDE_PLAIN        17
#define BG_TILE_OUTSIDE_RIGHT        18
#define BG_TILE_OUTSIDE_BOTTOM_LEFT  32
#define BG_TILE_OUTSIDE_BOTTOM       33
#define BG_TILE_OUTSIDE_BOTTOM_RIGHT 34

#define BG_TILE_GAME_FOOD            76
#define BG_TILE_GAME_EMPTY            8

#define BG_TILE_SNAKE_BASE            8
#define BG_TILE_SNAKE_FLAG_HEAD    0x80

/* snake tile index:   (8 bits) hfff1ttt
 * h (1 bit): is head
 * f (3 bits): Direction from
 * t (3 bits): Direction to
 */



typedef struct {
    uint8_t x;
    uint8_t y;
} Position;

typedef enum {
    EMPTY, FOOD, TAIL, HEAD
} CellState;
typedef enum {
    NONE, TOP, RIGHT, BOTTOM, LEFT
} Direction;

inline Direction reverseDir(Direction dir) {
    switch(dir) {
        case TOP:    return BOTTOM;
        case RIGHT:  return LEFT;
        case BOTTOM: return TOP;
        case LEFT:   return RIGHT;
        default:     return NONE;
    }
}


bn::random random;


template <unsigned int W, unsigned int H>
class RegularBackground {
    private:
        alignas(int) bn::regular_bg_map_cell cells[W*H];
        bn::regular_bg_map_item map_item;
    public:
        bn::regular_bg_item bg_item;
        bn::regular_bg_ptr bg;
        bn::regular_bg_map_ptr bg_map;


        RegularBackground(const bn::regular_bg_tiles_item& tiles_item, const bn::bg_palette_item& palette_item):
            map_item(cells[0], bn::size(W, H)),
            bg_item(tiles_item, palette_item, map_item),
            bg(bg_item.create_bg()),
            bg_map(bg.map())
        {
        }

        bn::regular_bg_map_cell_info get_cell_info(int x, int y) {
            return bn::regular_bg_map_cell_info(cells[map_item.cell_index(x, y)]);
        }

        void set_cell_info(int x, int y, bn::regular_bg_map_cell_info& info) {
            cells[map_item.cell_index(x, y)] = info.cell();
        }

};




class Snake {
    public:
        Position tail[SNAKE_MAX_SIZE]; // 0 is head
        uint16_t length;

        void reset() {
            length = 1;
            tail[0].x = random.get_unbiased_int(GAME_WIDTH) + GAME_X;
            tail[0].y = random.get_unbiased_int(GAME_HEIGHT) + GAME_Y;
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
        RegularBackground<32, 32> bg1;

        CellState cells[GAME_X + GAME_WIDTH][GAME_Y + GAME_HEIGHT];
        Direction fromDir[GAME_X + GAME_WIDTH][GAME_Y + GAME_HEIGHT];
        Direction toDir[GAME_X + GAME_WIDTH][GAME_Y + GAME_HEIGHT];
        

        Snake snake;
        Direction currentDirection = NONE;
        bool gameEnd;
        bool pause;
        bool fast;
        uint16_t maxScore = 0;

        uint32_t tickCount;
        uint16_t tickMod; // snake step when = 0
        
    public:

        Game():
            bg1(bn::regular_bg_tiles_items::bg, bn::regular_bg_tiles_items::bg_palette)
            {
                bg1.bg.set_top_left_position(0, 0);
                bg1.bg.set_priority(3);
                init();
                updateVRAM();
            }


        void setCell(Position p, CellState state, Direction from, Direction to) {
            cells[p.x][p.y] = state;
            fromDir[p.x][p.y] = from;
            toDir[p.x][p.y] = to;

            bn::regular_bg_map_cell_info cell_info = bg1.get_cell_info(p.x, p.y);
            int tile_i;
            switch (state)
            {
            case HEAD:
            case TAIL:
                tile_i = BG_TILE_SNAKE_BASE | ((from & 0b111) << 4) | (to & 0b111);
                if (state == HEAD)
                    tile_i |= BG_TILE_SNAKE_FLAG_HEAD;
                cell_info.set_tile_index(tile_i);
                break;
            case FOOD:
                cell_info.set_tile_index(BG_TILE_GAME_FOOD);
                break;
            default:
                cell_info.set_tile_index(BG_TILE_GAME_EMPTY);
                break;
            }
            bg1.set_cell_info(p.x, p.y, cell_info);
        }

        void setCell(Position p, CellState state) {
            setCell(p, state, fromDir[p.x][p.y], toDir[p.x][p.y]);
        }

        inline CellState getCell(Position p) {
            return cells[p.x][p.y];
        }

        void spawnFood() {
            Position food;
            do {
                food.x = random.get_unbiased_int(GAME_WIDTH) + GAME_X;
                food.y = random.get_unbiased_int(GAME_HEIGHT) + GAME_Y;
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

            // check input state
            if (bn::keypad::start_pressed()) {
                if (gameEnd) { // restart new game
                    init();
                }
                else {
                    pause = !pause;
                }
            }

            if (!pause) {
                fast = bn::keypad::a_held();

                Position &currPos = snake.tail[0];
                Direction &from = fromDir[currPos.x][currPos.y];
    
                if (bn::keypad::left_held() && from != LEFT) {
                    currentDirection = LEFT;
                }
                if (bn::keypad::right_held() && from != RIGHT) {
                    currentDirection = RIGHT;
                }
                if (bn::keypad::up_held() && from != TOP) {
                    currentDirection = TOP;
                }
                if (bn::keypad::down_held() && from != BOTTOM) {
                    currentDirection = BOTTOM;
                }
            }
        }




        void update() {
            if (gameEnd || pause)
                return;
            // update game state
            if (currentDirection == NONE) {
                setCell(snake.tail[0], HEAD, NONE, BOTTOM);
                return;
            }
            Position currPos = snake.tail[0];
            
            setCell(currPos, HEAD, fromDir[currPos.x][currPos.y], currentDirection);


            tickMod++;
            tickMod %= fast ? 6 : 30; // 0.1 or 0.5 s

            if (tickMod != 0)
                return;

            // UPDATE game state
            Position nextPos = currPos;
            if (currentDirection == LEFT)
                nextPos.x--;
            else if (currentDirection == RIGHT)
                nextPos.x++;
            else if (currentDirection == TOP)
                nextPos.y--;
            else if (currentDirection == BOTTOM)
                nextPos.y++;

            if (nextPos.x < GAME_X || nextPos.x >= GAME_X + GAME_WIDTH
                    || nextPos.y < GAME_Y || nextPos.y >= GAME_Y + GAME_HEIGHT) {
                        // snake sors de l'écran
                finish();
                return;
            }

            if (getCell(nextPos) == FOOD) {
                snake.forward(nextPos, true);
                setCell(nextPos, HEAD, reverseDir(currentDirection), currentDirection);
                setCell(snake.tail[1], TAIL);
                if (snake.length == SNAKE_MAX_SIZE) { // WIN
                    finish();
                    return;
                }
                // spawn new food
                spawnFood();
            }
            else if (getCell(nextPos) == TAIL) {
                gameEnd = true;
            }
            else { // forward normally
                snake.forward(nextPos, false);

                setCell(nextPos, HEAD, snake.length == 1 ? NONE : reverseDir(currentDirection), currentDirection);

                if (snake.length > 2)
                    setCell(snake.tail[1], TAIL);
                
                if (snake.length > 1) {
                    Position lastTailPos = snake.tail[snake.length - 1];
                    setCell(lastTailPos, TAIL, NONE, toDir[lastTailPos.x][lastTailPos.y]);
                }
                setCell(snake.tail[snake.length], EMPTY);
            }
        }



        void updateVRAM() {
            bg1.bg_map.reload_cells_ref();
        }




        void init() {

            // draw borders
            for (int x = 0; x < 30; x++) {
                for (int y = 0; y < 20; y++) {
                    bn::regular_bg_map_cell_info cell_info = bg1.get_cell_info(x, y);
                    if (x < GAME_X - 1 || x > GAME_X + GAME_WIDTH      // outside
                        || y < GAME_Y - 1 || y > GAME_Y + GAME_HEIGHT)
                        cell_info.set_tile_index(BG_TILE_OUTSIDE_PLAIN);
                    else if (x == GAME_X - 1) { // left border
                        if (y == GAME_Y - 1) // top left border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_TOP_LEFT);
                        else if (y == GAME_Y + GAME_HEIGHT) // bottom left border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_BOTTOM_LEFT);
                        else // rest of left border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_LEFT);
                    }
                    else if (x == GAME_X + GAME_WIDTH) { // right border
                        if (y == GAME_Y - 1) // top right border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_TOP_RIGHT);
                        else if (y == GAME_Y + GAME_HEIGHT) // bottom right border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_BOTTOM_RIGHT);
                        else // rest of right border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_RIGHT);
                    }
                    else {
                        if (y == GAME_Y - 1) // rest of top border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_TOP);
                        else if (y == GAME_Y + GAME_HEIGHT) // rest of bottom border
                            cell_info.set_tile_index(BG_TILE_OUTSIDE_BOTTOM);
                        else // game space
                            cell_info.set_tile_index(BG_TILE_GAME_EMPTY);
                    }
                    bg1.set_cell_info(x, y, cell_info);
                }
            }

            
            for (int i = 0; i < GAME_X + GAME_WIDTH; i++)
                for (int j = 0; j < GAME_Y + GAME_HEIGHT; j++)
                    cells[i][j] = EMPTY;

            snake.reset();
            setCell(snake.tail[0], HEAD);

            spawnFood();

            currentDirection = NONE;
            tickMod = 0;
            gameEnd = false;
            pause = false;
        }

        void tick() {

            handleInput();
            update();
            updateVRAM();
        }

};




int main() {
    bn::core::init();
    bn::bg_tiles::set_allow_offset(false);

    Game game;

    for(;;) {
        bn::core::update();
        game.tick();
    }
}
