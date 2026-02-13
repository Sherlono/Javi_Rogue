#ifndef JV_MAP_CLASSES_H
#define JV_MAP_CLASSES_H

#include "bn_point.h"
#include "bn_memory.h"
#include "bn_vector.h"
#include "bn_bg_tiles.h"
#include "bn_unique_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"

#include "jv_constants.h"

// For big tile maps for tiled bgs or other purposes
class GameMap{
public:
    using cell_type = uint16_t;

    ~GameMap(){ bn::memory::clear(size(), _data[0]);}
    GameMap(uint16_t x, uint16_t y): _data(std::make_unique<cell_type[]>(x*y)), _width(x), _height(y){}

    // Getters
    [[nodiscard]] uint16_t width() const {return _width;}
    [[nodiscard]] uint16_t height() const {return _height;}
    [[nodiscard]] int size() const {return _width*_height;}
    [[nodiscard]] bool horizontal_flip(int index) const {return _data[index] >= FLPTHD;}
    [[nodiscard]] cell_type cell(const int x, const int y) const {
        cell_type val = _data[x + y*_width];
        return val - FLPTHD*(val >= FLPTHD);
    }
    
    // Setters
    void set_cell(const int x, const int y, int value){
        _data[x + y*_width] = value;
    }
    
    void set_horizontal_flip(int index, bool f){
        if(f == true){
            if(horizontal_flip(index) == false){
                _data[index] += FLPTHD;
            }
        }else if(horizontal_flip(index) == true){
            _data[index] -= FLPTHD;
        }
    }

    void insert_data(const int map_x, const int map_y, cell_type* map, const bn::point top_left, const bool fliped = false){
        int y_begin = top_left.y()  ,   x_begin = top_left.x();
        int x_end = x_begin + map_x   ,   y_end = y_begin + map_y;

        for(int y = y_begin; y < y_end; y++){
            int room_index, map_index;
            for(int x = x_begin; x < x_end; x++){
                if(!fliped){
                    room_index = (x - x_begin) + (y - y_begin) * map_x;
                }else{
                    room_index = -(x + 1 - x_end) + (y - y_begin) * map_x;
                }
                if(!map[room_index]){ continue;}
                map_index = x + y * this->_width;
                this->_data[map_index] = map[room_index];
                this->set_horizontal_flip(map_index, fliped ? !(map[room_index] >= FLPTHD) : (map[room_index] >= FLPTHD));
            }
        }
    }

    void reset(){
        bn::memory::clear(size(), _data[0]);
    }

    std::unique_ptr<cell_type[]> _data;
    //static constexpr cell_type FLPTHD = 127;
    static constexpr cell_type FLPTHD = 32767;  // Flip threshold
private:
    const uint16_t _width, _height;
};

// Tile data for tiled regular bgs
struct bg_map
{
    static constexpr int columns = 32;
    static constexpr int rows = 32;
    static constexpr int cells_count = columns * rows;

    alignas(int) bn::regular_bg_map_cell cells[cells_count];
    bn::regular_bg_map_item map_item;

    bg_map(): map_item(cells[0], bn::size(bg_map::columns, bg_map::rows)){ reset();}

    // Getters
    int cell(const int x, const int y){ return cells[x + y*32];}

    // Setters
    void set_cell(const int x, const int y, GameMap::cell_type value, bool flip = false){
        /*BN_ASSERT(x >= 0, "Invalid x", x);
        BN_ASSERT(y >= 0, "Invalid y", y);*/

        bn::regular_bg_map_cell& current_cell = cells[map_item.cell_index(x, y)];
        bn::regular_bg_map_cell_info current_cell_info(current_cell);

        if(current_cell_info.cell() != value){
            current_cell_info.set_tile_index(value);
            if(current_cell_info.horizontal_flip() != flip){ current_cell_info.set_horizontal_flip(flip);}
        }
        //current_cell_info.set_palette_id(0);
        current_cell = current_cell_info.cell();
    }

    void reset(){
        bn::memory::clear(cells_count, cells[0]);
    }
};

// Level layout cells
struct Zone{
    ~Zone(){ reset();}
    Zone(uint8_t w, uint8_t h, bool* a):_width(w), _height(h), data(a){
        bn::memory::clear(size(), data[0]);
    }
    
    bool cell(int x, int y) { 
        if(x < 0 || y < 0 || x >= _width || y >= _height){ // If xy is outside the zone
            return false;
        }else{
            return data[x + (y*_width)];
        }
    }
    
    int size() const { return _width*_height;}
    void reset(){
        bn::memory::clear(size(), data[0]);
    }
    const uint8_t _width, _height;
    bool* data;
};

#endif