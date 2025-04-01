#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_log.h"
#include "bn_math.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_memory.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"

#include "bn_bg_palette_items_floor_palette.h"
#include "bn_regular_bg_tiles_items_floor_tiles.h"

#include "jv_constants.h"
#include "jv_interface.h"

struct game_map{
    game_map(uchar_t x, uchar_t y, uchar_t* blocks, bool* flips):width(x), height(y), _blocks(blocks), _flips(flips){}

    // Methods
    [[nodiscard]] uchar_t x() const {return width;}
    [[nodiscard]] uchar_t y() const {return height;}
    [[nodiscard]] int size() const {return width*height;}
    [[nodiscard]] uchar_t cell(const int x, const int y) const {return _blocks[x + (y*width)];}

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
    cuchar_t width, height;
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
            if(current_cell_info.horizontal_flip() != flip){ current_cell_info.set_horizontal_flip(flip);}
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
    template <typename PointsVector, typename DataArray>
    void random_coords(PointsVector& points_out, bn::random& randomizer, DataArray& map, const int width, const int height);
    // This function is meant to be replaced with procedural generation
    template <typename PointsVector>
    void LevelFactory(game_map& map, const int option, PointsVector& start_points, bn::random& randomizer){
        int width, height;
        switch(option){
            case 1:{
                width = 20;
                height = 20;
                uchar_t blockArr[400] = {
                     0, 0,31,28,31, 0, 0, 0, 0,31,28,28,28,28,28,31, 0, 0, 0, 0,
                    36,25,29,22,29,25,28,28,25,29,22,22,22,22,22,29,34, 0, 0, 0,
                    35,24,32, 4,32,24,22,22,24,32, 3, 2, 2, 2, 3,33,38,28,31, 0,
                    35,21,18,20,18, 6, 2, 2, 6,18,20,20,20,20,20, 1,22,22,29,34,
                    35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 8, 8,33,35,
                    35, 1,20,20,20,20,20,13,10,20,10,13,20,20,20, 1,42,42, 7,35,
                    37,17,11,10,20,10,11,17,38, 7,42,17,11,20,11,17,37,35, 7,35,
                     0,41,40,42, 7,22,22,22,22, 7,35,41,42, 7,42,41, 0,35, 7,35,
                     0, 0, 0,35,20, 8, 8, 8, 8,20,38,28,38, 7,35, 0, 0,35, 7,35,
                     0, 0, 0,35, 7,42,40,40,42, 7,22,22,22, 7,35, 0, 0,35, 7,35,
                     0, 0, 0,35, 7,35, 0,31,38,20, 8, 8, 8,20,35, 0, 0,35, 7,35,
                     0, 0, 0,35, 7,35,34,29,22, 7,22,29,42, 7,35, 0, 0,35, 7,35,
                     0,31,28,38, 7,38,38,33, 3,20, 3,33,39, 7,38,28,31,35, 7,35,
                    34,29,22,22, 7,22,22, 1,20,20,20, 1,22, 7,22,22,29,35, 7,35,
                    35,33, 3, 2,20, 2, 2, 5,20,20,20, 5, 2,20, 2, 3,33,39, 7,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 1,22, 7,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 8,17,37,
                    35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 1,42,41, 0,
                    37,17,11,10,10,10,10,10,10,10,10,10,10,10,10,11,17,37, 0, 0,
                     0,41,40,40,40,40,40,40,40,40,40,40,40,40,40,40,41, 0, 0, 0 };
                random_coords(start_points, randomizer, blockArr, width, height);
                bool flipArr[400] = {
                     0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
                     0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
                     0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
                     0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
                     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
                     0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
                     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1,
                     0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
                     0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        FloorFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                    }
                }
                break;
                }
            case 2:{
                width = 10;
                height = 11;
                uchar_t blockArr[110] = {
                     0,31,28,28,28,28,28,28,31, 0,
                    34,29,22,22,22,22,22,22,29,34,
                    35,33, 3, 2, 2, 2, 2, 3,33,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20, 1,35,
                    37,17,11,10,10,10,10,11,17,37,
                     0,41,40,40,40,40,40,40,41, 0 };
                random_coords(start_points, randomizer, blockArr, width, height);
                bool flipArr[110] = {
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 1, 0, 1, 0, 1, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };

                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        FloorFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                    }
                }
                break;
                }
            default:{
                width = 12;
                height = 10;
                uchar_t blockArr[120] = {
                     0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
                     6, 6, 7, 7, 8, 8, 9, 9,10,10,11,11,
                    12,12,13,13,14,14,15,15,16,16,17,17,
                    18,18,19,19,20,20,21,21,22,22,23,23,
                    24,24,25,25,26,26,27,27,28,28,29,29,
                    30,30,31,31,32,32,33,33,34,34,35,35,
                    36,36,37,37,38,38,39,39,40,40,41,41,
                    42,42,43,43,44,44,45,45,46,46,47,47,
                    48,48,49,49,50,51,52,53,54,55,56,57,
                    58,59,60,61,62,63,64,65,66,67,68,69};
                random_coords(start_points, randomizer, blockArr, width, height);
                bool flipArr[120] = {
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
    
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        FloorFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                    }
                }
            }
            break;
        }
    }
}
#endif