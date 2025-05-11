#include "regular_bg_helper.h"


#include <bn_bg_tiles.h>
#include <bn_core.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_string.h>


#include "bn_regular_bg_tiles_items_bg.h"




#define GAME_X 1
#define GAME_Y 2
#define GAME_WIDTH 28
#define GAME_HEIGHT 17
#define SNAKE_MAX_SIZE GAME_HEIGHT*GAME_WIDTH



typedef enum {
    EMPTY, FOOD, TAIL, HEAD
} CellState;

typedef enum {
    NONE, TOP, RIGHT, BOTTOM, LEFT
} Direction;



namespace tileset {
    namespace bg {

        constexpr static int _tileset_width = 16;

        constexpr static int transparent = 0x00;

        constexpr static int dirt[] = {0x10, 0x20};

        namespace grass {
            constexpr static int top_left     = 0x01;
            constexpr static int top          = 0x02;
            constexpr static int top_right    = 0x03;
            constexpr static int left         = 0x11;
            constexpr static int plain        = 0x12;
            constexpr static int right        = 0x13;
            constexpr static int bottom_left  = 0x21;
            constexpr static int bottom       = 0x22;
            constexpr static int bottom_right = 0x23;
        }

        constexpr static int food = 0x4b;

        #define BG_TILE_SNAKE_BASE         0x08
        #define BG_TILE_SNAKE_FLAG_HEAD    0x04
        constexpr inline static int snake(bool head, Direction from, Direction to) {
            return BG_TILE_SNAKE_BASE
                    | (head ? BG_TILE_SNAKE_FLAG_HEAD : 0)
                    | ((from & 0b111) << 4)
                    | ((to - 1) & 0b11);
        }
        
        namespace text {
            constexpr static regular_bg_tile_rect<4, 2> score(4, 0, _tileset_width);
            constexpr static regular_bg_tile_rect<3, 2> top(10, 5, _tileset_width);

            constexpr static regular_bg_tile_rect<6, 2> pause(0, 3, _tileset_width);
            constexpr static regular_bg_tile_rect<10, 2> game_over(0, 7, _tileset_width);

            constexpr static regular_bg_tile_rect<3, 2> key_a(13, 5, _tileset_width);
            constexpr static regular_bg_tile_rect<5, 2> key_start(0, 9, _tileset_width);
            constexpr static regular_bg_tile_rect<3, 2> key_arrows(0, 11, _tileset_width);

            constexpr static regular_bg_tile_rect<6, 2> speed_up(10, 7, _tileset_width);
            constexpr static regular_bg_tile_rect<5, 2> resume(5, 9, _tileset_width);
            constexpr static regular_bg_tile_rect<5, 2> restart(10, 9, _tileset_width);
            constexpr static regular_bg_tile_rect<8, 2> move_snake(3, 11, _tileset_width);

            constexpr inline static regular_bg_tile_rect<1, 2> digits[] = {
                regular_bg_tile_rect<1, 2>(0, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(1, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(2, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(3, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(4, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(5, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(6, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(7, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(8, 5, _tileset_width),
                regular_bg_tile_rect<1, 2>(9, 5, _tileset_width)
            };
    
            template<unsigned int bgW, unsigned int bgH, unsigned int nbDigit>
            void regular_bg_print_number(regular_bg<bgW, bgH>& bg, bn::point topLeft, unsigned int number, bool padZero) {
                bn::string<nbDigit> str = bn::to_string<nbDigit>(number);
                while (!str.full()) {
                    str = bn::string<nbDigit>(padZero ? "0" : " ") + str;
                }
                for (unsigned int i = 0; i < nbDigit; i++) {
                    char c = str.at(i);
                    if (c >= '0' && c <= '9') {
                        bg.set_cells_rect(topLeft.x() + i, topLeft.y(), digits[c - '0']);
                    }
                    else {
                        bg.clear_cells_rect(topLeft.x() + i, topLeft.y(), 1, 2, regular_bg_map_cell(tileset::bg::transparent));
                    }
                }
                
            }
        }
    }

}

namespace bg {
    class ground: public regular_bg<32, 32> {
    public:
        ground():
            regular_bg(bn::regular_bg_tiles_items::bg, bn::regular_bg_tiles_items::bg_palette)
        {
            bg.set_top_left_position(0, 0);
            bg.set_priority(3);

            
            // draw borders
            bn::regular_bg_map_cell cell;
            for (int x = 0; x < 30; x++) {
                for (int y = 0; y < 20; y++) {

                    if (x < GAME_X - 1 || x > GAME_X + GAME_WIDTH      // outside
                        || y < GAME_Y - 1 || y > GAME_Y + GAME_HEIGHT)
                        cell = regular_bg_map_cell(tileset::bg::grass::plain);
                    else if (x == GAME_X - 1) { // left border
                        if (y == GAME_Y - 1) // top left border
                            cell = regular_bg_map_cell(tileset::bg::grass::top_left);
                        else if (y == GAME_Y + GAME_HEIGHT) // bottom left border
                            cell = regular_bg_map_cell(tileset::bg::grass::bottom_left);
                        else // rest of left border
                            cell = regular_bg_map_cell(tileset::bg::grass::left);
                    }
                    else if (x == GAME_X + GAME_WIDTH) { // right border
                        if (y == GAME_Y - 1) // top right border
                            cell = regular_bg_map_cell(tileset::bg::grass::top_right);
                        else if (y == GAME_Y + GAME_HEIGHT) // bottom right border
                            cell = regular_bg_map_cell(tileset::bg::grass::bottom_right);
                        else // rest of right border
                            cell = regular_bg_map_cell(tileset::bg::grass::right);
                    }
                    else {
                        if (y == GAME_Y - 1) // rest of top border
                            cell = regular_bg_map_cell(tileset::bg::grass::top);
                        else if (y == GAME_Y + GAME_HEIGHT) // rest of bottom border
                            cell = regular_bg_map_cell(tileset::bg::grass::bottom);
                        else { // game space
                            cell = regular_bg_map_cell(tileset::bg::dirt[(x + y) & 1],
                                    ((x >> 2) + (y >> 1)) & 1,
                                    ((x >> 1) + (y >> 2)) & 1);
                        }
                    }
                    set_cell(x, y, cell);
                }
            }
            
            updateVRAM();
        }
    };


    class game: public regular_bg<32, 32> {
        public:
        game():
            regular_bg(bn::regular_bg_tiles_items::bg, bn::regular_bg_tiles_items::bg_palette)
        {
            bg.set_top_left_position(0, 0);
            bg.set_priority(2);
            
            set_cells_rect(1, 0, tileset::bg::text::score);
            set_cells_rect(23, 0, tileset::bg::text::top);

            drawScores(0, 0);
        }

        void drawScores(unsigned int currentScore, unsigned int maxScore) {
            tileset::bg::text::regular_bg_print_number<32, 32, 3>(*this, bn::point(5, 0), currentScore, true);
            tileset::bg::text::regular_bg_print_number<32, 32, 3>(*this, bn::point(26, 0), maxScore, true);
        }
    };

    class overlay: public regular_bg<32, 32> {
        public:
        overlay():
            regular_bg(bn::regular_bg_tiles_items::bg, bn::regular_bg_tiles_items::bg_palette)
        {
            bg.set_top_left_position(0, 0);
            bg.set_priority(1);
        }

        void clear() {
            clear_cells_rect(0, 0, 30, 20, tileset::bg::transparent);
        }


        void game_over() {
            set_cells_rect(10, 5, tileset::bg::text::game_over);
            
            set_cells_rect(10, 8, tileset::bg::text::key_start);
            set_cells_rect(15, 8, tileset::bg::text::restart);
        }

        void pause() {
            set_cells_rect(12, 5, tileset::bg::text::pause);

            set_cells_rect(10, 8, tileset::bg::text::key_start);
            set_cells_rect(15, 8, tileset::bg::text::resume);

            set_cells_rect(12, 10, tileset::bg::text::key_arrows);
            set_cells_rect(15, 10, tileset::bg::text::move_snake);
            
            set_cells_rect(12, 12, tileset::bg::text::key_a);
            set_cells_rect(15, 12, tileset::bg::text::speed_up);
        }
    };
}




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






class Snake {
    public:
        bn::point tail[SNAKE_MAX_SIZE]; // 0 is head
        uint16_t length;

        void reset() {
            length = 1;
            tail[0] = bn::point(random.get_unbiased_int(GAME_WIDTH) + GAME_X,
                    random.get_unbiased_int(GAME_HEIGHT) + GAME_Y);
        }
    
        void forward(bn::point nextPos, bool grow) {
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
        bg::ground bgGround;
        bg::game bgGame;
        bg::overlay bgOverlay;

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
            bgGround(),
            bgGame(),
            bgOverlay()
            {
                init();
                updateVRAM();
            }


        void setCell(bn::point p, CellState state, Direction from, Direction to) {
            cells[p.x()][p.y()] = state;
            fromDir[p.x()][p.y()] = from;
            toDir[p.x()][p.y()] = to;

            bn::regular_bg_map_cell cell;
            switch (state)
            {
            case HEAD:
            case TAIL:
                cell = regular_bg_map_cell(tileset::bg::snake(state == HEAD, from, to));
                break;
            case FOOD:
                cell = regular_bg_map_cell(tileset::bg::food);
                break;
            default:
                cell = regular_bg_map_cell(tileset::bg::transparent);
                break;
            }
            bgGame.set_cell(p, cell);
        }

        void setCell(bn::point p, CellState state) {
            setCell(p, state, fromDir[p.x()][p.y()], toDir[p.x()][p.y()]);
        }

        inline CellState getCell(bn::point p) {
            return cells[p.x()][p.y()];
        }

        void spawnFood() {
            bn::point food;
            do {
                food = bn::point(random.get_unbiased_int(GAME_WIDTH) + GAME_X,
                        random.get_unbiased_int(GAME_HEIGHT) + GAME_Y);
            } while (getCell(food) != EMPTY);
            setCell(food, FOOD);
        }

        

        void finish() {
            gameEnd = true;
            if (snake.length > maxScore) {
                maxScore = snake.length;
            }
            drawScores();
            bgOverlay.game_over();
        }


        

        void handleInput() {

            // check input state
            if (bn::keypad::start_pressed()) {
                if (gameEnd) { // restart new game
                    init();
                }
                else {
                    pause = !pause;
                    if (pause) {
                        bgOverlay.pause();
                    }
                    else {
                        bgOverlay.clear();
                    }
                }
            }

            if (!pause) {
                fast = bn::keypad::a_held();

                bn::point &currPos = snake.tail[0];
                Direction &from = fromDir[currPos.x()][currPos.y()];
    
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
            bn::point currPos = snake.tail[0];
            
            setCell(currPos, HEAD, fromDir[currPos.x()][currPos.y()], currentDirection);


            tickMod++;
            tickMod %= fast ? 6 : 30; // 0.1 or 0.5 s

            if (tickMod != 0)
                return;

            // UPDATE game state
            bn::point nextPos = currPos;
            if (currentDirection == LEFT)
                nextPos += bn::point(-1, 0);
            else if (currentDirection == RIGHT)
                nextPos += bn::point(1, 0);
            else if (currentDirection == TOP)
                nextPos += bn::point(0, -1);
            else if (currentDirection == BOTTOM)
                nextPos += bn::point(0, 1);

            if (nextPos.x() < GAME_X || nextPos.x() >= GAME_X + GAME_WIDTH
                    || nextPos.y() < GAME_Y || nextPos.y() >= GAME_Y + GAME_HEIGHT) {
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
                drawScores();
                // spawn new food
                spawnFood();
            }
            else if (getCell(nextPos) == TAIL) {
                finish();
                return;
            }
            else { // forward normally
                snake.forward(nextPos, false);

                setCell(nextPos, HEAD, snake.length == 1 ? NONE : reverseDir(currentDirection), currentDirection);

                if (snake.length > 2)
                    setCell(snake.tail[1], TAIL);
                
                if (snake.length > 1) {
                    bn::point lastTailPos = snake.tail[snake.length - 1];
                    setCell(lastTailPos, TAIL, NONE, toDir[lastTailPos.x()][lastTailPos.y()]);
                }
                setCell(snake.tail[snake.length], EMPTY);
            }

        }



        inline void drawScores() {
            bgGame.drawScores(snake.length, maxScore);
        }



        void updateVRAM() {
            bgGame.updateVRAM();
            bgOverlay.updateVRAM();
        }




        void init() {

            bgOverlay.clear();
            
            for (int i = GAME_X; i < GAME_X + GAME_WIDTH; i++)
                for (int j = GAME_Y; j < GAME_Y + GAME_HEIGHT; j++)
                    setCell(bn::point(i, j), EMPTY);

            snake.reset();
            setCell(snake.tail[0], HEAD);

            spawnFood();

            currentDirection = NONE;
            tickMod = 0;
            gameEnd = false;
            pause = false;
            
            drawScores();
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
