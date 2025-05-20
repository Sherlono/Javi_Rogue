#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_vector.h"
#include "bn_random.h"
#include "bn_memory.h"
#include "bn_assert.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"

#include "jv_fog.h"
#include "jv_constants.h"

class game_map{
public:
    ~game_map(){ bn::memory::clear(size(), _blocks[0]);}
    game_map(uint8_t x, uint8_t y, uint8_t* blocks):width(x), height(y), _blocks(blocks){}

    // Getters
    [[nodiscard]] uint8_t x() const {return width;}
    [[nodiscard]] uint8_t y() const {return height;}
    [[nodiscard]] int size() const {return width*height;}
    [[nodiscard]] bool horizontal_flip(int index) const {return _blocks[index] >= 127;}
    [[nodiscard]] uint8_t cell(int x, int y) const {
        uint8_t val = _blocks[x + y*width];
        return val - 127*(val >= 127);
    }

    uint8_t operator[](int index){
        BN_ASSERT(index >= 0, "Invalid index", index);
        uint8_t val = _blocks[index];
        return val - 127*(val >= 127);
    }

    // Setters
    void set_horizontal_flip(int index, bool f){
        if(f == true){
            if(horizontal_flip(index) == false){
                _blocks[index] += 127;
            }
        }else if(horizontal_flip(index) == true){
            _blocks[index] -= 127;
        }
    }

    void insert_map(const game_map room, const bn::point top_left, const bool fliped = false){
        int y_begin = top_left.y()  ,   x_begin = top_left.x();
        int aux_y = y_begin + room.height   ,   aux_x = x_begin + room.width;
        int y_end = aux_y * (aux_y < height) + height * (aux_y >= height)   ,   x_end = aux_x * (aux_x < width) + width * (aux_x >= width);

        for(int y = y_begin; y < y_end; y++){
            if(!fliped){
                for(int x = x_begin; x < x_end; x++){
                    int map_index = x + y * this->width;
                    int room_index = (x - x_begin) + (y - y_begin) * room.width;
                    if(!room._blocks[room_index]){ continue;}
                    this->_blocks[map_index] =  room._blocks[room_index];
                    this->set_horizontal_flip(map_index, room.horizontal_flip(room_index));
                }
            }else{
                for(int x = x_end - 1; x >= x_begin; x--){
                    int map_index = x + y * this->width;
                    int room_index = -(x + 1 - x_end) + (y - y_begin) * room.width;
                    if(!room._blocks[room_index]){ continue;}
                    this->_blocks[map_index] = room._blocks[room_index];
                    this->set_horizontal_flip(map_index, !room.horizontal_flip(room_index));
                }
            }
        }
    }
    
    void reset(){
        bn::memory::clear(size(), _blocks[0]);
    }
private:
    // Members
    const uint8_t width, height;
    uint8_t *_blocks;
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
        BN_ASSERT(position.x() >= 0, "Invalid x", position.x());
        BN_ASSERT(position.y() >= 0, "Invalid y", position.y());

        bn::regular_bg_map_cell& current_cell = cells[map_item.cell_index(position.x(), position.y())];
        bn::regular_bg_map_cell_info current_cell_info(current_cell);
        if(current_cell_info.cell() != value){
            current_cell_info.set_tile_index(value);
            if(current_cell_info.horizontal_flip() != flip){ current_cell_info.set_horizontal_flip(flip);}
        }
        
        current_cell = current_cell_info.cell();
    }

    void reset(){
        bn::memory::clear(cells_count, cells[0]);
    }
};

struct Zone{
    ~Zone(){ reset();}
    Zone(uint8_t w, uint8_t h, bool* a):_width(w), _height(h), data(a){
        bn::memory::clear(size(), data[0]);
    }
    
    bool cell(int x, int y) { 
        if(x < 0 || y < 0 || x >= _width || y >= _height){
            return false;
        }else{
            return data[x + (y*_width)];
        }
    }
    bool marginal(int x, int y){
        if(x < 0 || y < 0 || x >= _width || y >= _height){
            return true;
        }
        return false;
    }

    void get_empty_neighbours(int cell_x, int cell_y, bn::vector<bn::point, 8> neighbours){
        int start_x = bn::max(cell_x-1, 0), start_y = bn::max(cell_y-1, 0);
        int end_x = bn::min(cell_x+1, int(_width)), end_y = bn::min(cell_y+1, int(_height));

        for(int y = start_y; y < end_y; y++){
            for(int x = start_x; x < end_x; x++){
                if(!(x == start_x+1 && y == start_y+1) && !cell(x, y)){
                    neighbours.push_back(bn::point(x, y));
                }
            }
        }
    }

    int size() const { return _width*_height;}
    void reset(){
        bn::memory::clear(size(), data[0]);
    }
    const uint8_t _width, _height;
    bool* data;
};

namespace jv{
    void BlockFactory(game_map& map, const bn::point top_left, const uint8_t option, const bool blockFlip);
    bn::point InsertRoom(game_map& map, const bn::point top_left, const uint8_t option, Fog* fog_ptr = NULL);
    void GenerateLevel(game_map& map, Zone& zone, bn::random& randomizer, Fog* fog_ptr = NULL);
}
#endif