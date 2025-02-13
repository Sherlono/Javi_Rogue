#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

//#include "bn_log.h"
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

namespace jv{

}

struct bg_map
{
    static constexpr int columns = 32;
    static constexpr int rows = 32;
    static constexpr int cells_count = columns * rows;

    alignas(int) bn::regular_bg_map_cell cells[cells_count];
    bn::regular_bg_map_item map_item;

    bg_map() :
        map_item(cells[0], bn::size(bg_map::columns, bg_map::rows))
    {
        reset();
    }

    void set_cell(int x, int y, uint16_t value, bool flip = false)
    {
        bn::regular_bg_map_cell& current_cell = cells[map_item.cell_index(x, y)];
        bn::regular_bg_map_cell_info current_cell_info(current_cell);
        if(current_cell_info.cell() != value){
            current_cell_info.set_tile_index(value);
        }
        if(current_cell_info.horizontal_flip() != flip){
            current_cell_info.set_horizontal_flip(flip);
        }
        //current_cell_info.set_palette_id(1);
        
        current_cell = current_cell_info.cell();
    }

    void insert_block(int width, int height, const auto block, const auto tileFlip, bool blockFlip, const bn::point top_left){
        int y_begin = top_left.y()*height  ,   x_begin = top_left.x()*width;
        int aux_y = y_begin + height   ,   aux_x = x_begin + width;
        int y_end = aux_y * (aux_y < 32) + 32 * (aux_y >= 32)   ,   x_end = aux_x * (aux_x < 32) + 32 * (aux_x >= 32);
        if(!blockFlip){
            for(int y = y_begin; y < y_end; y++){
                for(int x = x_begin; x < x_end; x++){
                    int room_index = (x - x_begin) + (y - y_begin) * width;
                    //BN_LOG("x: ", x, " y: ", y, " index: ", room_index);
                    this->set_cell(x, y, block[room_index], tileFlip[room_index]);
                }
            }
        }else{
            for(int y = y_begin; y < y_end; y++){
                for(int x = x_end-1; x >= x_begin; x--){
                    int room_index = -(x - x_end) + (y - y_begin) * width - 1;
                    //BN_LOG("x: ", x, " y: ", y, " index: ", room_index);
                    this->set_cell(x, y, block[room_index], !tileFlip[room_index]);
                }
            }
        }
    }

    void reset()
    {
        bn::memory::clear(cells_count, cells[0]);
    }

private:

};

#endif