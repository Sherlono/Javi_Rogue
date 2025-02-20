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

    void insert_block(int width, int height, const auto block, const auto tileFlip, bool blockFlip, const bn::point top_left, const uchar_t crop, const uchar_t side){
        int y_begin = top_left.y()*height   ,   x_begin = top_left.x()*width;
        int aux_y = y_begin + height        ,   aux_x = x_begin + width;
        int y_end = aux_y * (aux_y < 32) + 32 * (aux_y >= 32)   ,   x_end = aux_x * (aux_x < 32) + 32 * (aux_x >= 32);
        
        if(crop == jv::Side::left){
            width = 2;
            if(side == jv::Side::right){
                x_end = x_begin + 2;
            }else if(side == jv::Side::left){
                x_begin = x_end - 2;
            }
        }else if(crop == jv::Side::right){
            width = 2;
            if(side == jv::Side::right){
                x_begin = x_end - 2;
            }else if(side == jv::Side::left){
                x_end = x_begin + 2;
            }
        }
        
        for(int y = y_begin; y < y_end; y++){
            if(!blockFlip){
                for(int x = x_begin; x < x_end; x++){
                    if(side == jv::Side::down && x >= x_begin + 2){
                        continue;
                    }
                    int cell_index = (x - x_begin) + (y - y_begin)*width + 2*(crop != 0)*((y - y_begin) + 1*(crop == jv::Side::right)) + (side==jv::Side::left)*(2*(crop==jv::Side::left) - 2*(crop==jv::Side::right));
                    this->set_cell(x, y, block[cell_index], tileFlip[cell_index]);
                }
            }else{
                for(int x = x_end - 1; x >= x_begin; x--){
                    if(side == jv::Side::down && x >= x_end - 3){
                        continue;
                    }
                    int cell_index = -(x + 1 - x_end) + (y - y_begin)*width + 2*(crop != 0) * ((y - y_begin) + 1*(crop == jv::Side::left)) + (side==jv::Side::left)*(2*(crop==jv::Side::right) - 2*(crop==jv::Side::left));
                    this->set_cell(x, y, block[cell_index], !tileFlip[cell_index]);
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

namespace jv{
    void FloorFactory(const bn::point top_left, const uchar_t option, bool blockFlip, bn::unique_ptr<bg_map>& bg_map_ptr, const uchar_t crop = 0, const uchar_t side = 0);
    }
#endif