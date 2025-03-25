#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_log.h"
#include "bn_math.h"
#include "bn_memory.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"

#include "bn_bg_palette_items_floor_palette.h"
#include "bn_regular_bg_tiles_items_floor_tiles.h"

#include "jv_constants.h"


struct game_map{
    game_map(int x, int y, uchar_t* blocks, bool* flips):width(x), height(y), _blocks(blocks), _flips(flips){}

    // Methods
    [[nodiscard]] int x(){return width;}
    [[nodiscard]] int y(){return height;}
    [[nodiscard]] int size() const {return width * height;}

    // Insert room into the main map starting by the top left corner
    void insert_room(const game_map room, const bn::point top_left, const bool fliped = false){
        int y_begin = top_left.y()  ,   x_begin = top_left.x();
        int aux_y = y_begin + room.height   ,   aux_x = x_begin + room.width;
        int y_end = aux_y * (aux_y < height) + height * (aux_y >= height)   ,   x_end = aux_x * (aux_x < width) + width * (aux_x >= width);

        for(int y = y_begin; y < y_end; y++){
            if(!fliped){
                for(int x = x_begin; x < x_end; x++){
                    int map_index = x + y * this->width;
                    int room_index = (x - x_begin) + (y - y_begin) * room.width;
                    this->_blocks[map_index] =  room._blocks[room_index];
                    this->_flips[map_index] =  room._flips[room_index];
                }
            }else{
                for(int x = x_end - 1; x >= x_begin; x--){
                    int map_index = x + y * this->width;
                    int room_index = -(x + 1 - x_end) + (y - y_begin) * room.width;
                    this->_blocks[map_index] = room._blocks[room_index];
                    this->_flips[map_index] = !room._flips[room_index];
                }
            }
        }
    }
    // Members
    const int width, height;
    uchar_t *_blocks;
    bool *_flips;
};

struct bg_map
{
    static constexpr int columns = 32;
    static constexpr int rows = 32;
    static constexpr int cells_count = columns * rows;

    alignas(int) bn::regular_bg_map_cell cells[cells_count];
    bn::regular_bg_map_item map_item;

    bg_map(): map_item(cells[0], bn::size(bg_map::columns, bg_map::rows)){ reset();}

    void set_cell(bn::point position, uint16_t value, bool flip = false){
        bn::regular_bg_map_cell& current_cell = cells[map_item.cell_index(position.x(), position.y())];
        bn::regular_bg_map_cell_info current_cell_info(current_cell);
        if(current_cell_info.cell() != value){
            current_cell_info.set_tile_index(value);

            if(current_cell_info.horizontal_flip() != flip){
                current_cell_info.set_horizontal_flip(flip);
            }
        }
        //current_cell_info.set_palette_id(1);
        
        current_cell = current_cell_info.cell();
    }

    void reset()
    {
        bn::memory::clear(cells_count, cells[0]);
    }
};

namespace jv{
    void FloorFactory(game_map& map, const bn::point top_left, const uchar_t option, const bool blockFlip);
}
#endif