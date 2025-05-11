#ifndef REGULAR_BG_HELPER_H
#define REGULAR_BG_HELPER_H


#include <bn_regular_bg_map_cell.h>
#include <bn_regular_bg_map_item.h>
#include <bn_regular_bg_map_ptr.h>
#include <bn_regular_bg_item.h>
#include <bn_regular_bg_ptr.h>



template <unsigned int W, unsigned int H>
struct regular_bg_tile_rect {
    bn::regular_bg_map_cell cells[W][H];

    constexpr regular_bg_tile_rect() = default;
    
    constexpr regular_bg_tile_rect(int left, int top, int tileset_width) {
        for (unsigned int x = 0; x < W; x++) {
            for (unsigned int y = 0; y < H; y++) {
                cells[x][y] = ((left + x) + (top + y) * tileset_width) & 0x3FF;
            }
        }
    }
};



/// @brief better version of bn::regular_bg_map_cell_info, with full "constexpr" support and less variable copying
/// @param tile_index 
/// @param horizontal_flip 
/// @param vertical_flip 
/// @param palette_id 
/// @return 
constexpr bn::regular_bg_map_cell regular_bg_map_cell(unsigned int tile_index, bool horizontal_flip = false, bool vertical_flip = false, unsigned int palette_id = 0) {
    return (tile_index & 0x03ff)
        | (horizontal_flip ? 0x0400 : 0)
        | (vertical_flip ? 0x0800 : 0)
        | (palette_id & 0xf << 12);
}






template <unsigned int W, unsigned int H>
class regular_bg {
    private:
        alignas(int) bn::regular_bg_map_cell cells[W*H];
        bn::regular_bg_map_item map_item;
    public:
        bn::regular_bg_item bg_item;
        bn::regular_bg_ptr bg;
        bn::regular_bg_map_ptr bg_map;


        regular_bg(const bn::regular_bg_tiles_item& tiles_item, const bn::bg_palette_item& palette_item):
            map_item(cells[0], bn::size(W, H)),
            bg_item(tiles_item, palette_item, map_item),
            bg(bg_item.create_bg()),
            bg_map(bg.map())
        {
        }

        inline void set_cell(int x, int y, const bn::regular_bg_map_cell& cell) {
            cells[map_item.cell_index(x, y)] = cell;
        }

        inline void set_cell(bn::point p, const bn::regular_bg_map_cell& cell) {
            cells[map_item.cell_index(p)] = cell;
        }

        template<unsigned int RW, unsigned int RH>
        void set_cells_rect(int map_x, int map_y, const regular_bg_tile_rect<RW, RH> tile_rect) {
            for (unsigned int x = 0; x < RW; x++) {
                for (unsigned int y = 0; y < RH; y++) {
                    set_cell(x + map_x, y + map_y, tile_rect.cells[x][y]);
                }
            }
        }

        void clear_cells_rect(int map_x, int map_y, int w, int h, const bn::regular_bg_map_cell& cell) {
            for (int x = 0; x < w; x++) {
                for (int y = 0; y < h; y++) {
                    set_cell(x + map_x, y + map_y, cell);
                }
            }
        }

        inline void updateVRAM() {
            bg_map.reload_cells_ref();
        }

};




#endif
