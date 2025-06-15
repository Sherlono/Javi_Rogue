#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_rect.h"
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
    ~game_map();
    game_map(uint8_t x, uint8_t y, uint8_t* blocks);

    // Getters
    [[nodiscard]] uint8_t x() const;
    [[nodiscard]] uint8_t y() const;
    [[nodiscard]] int size() const;
    [[nodiscard]] bool horizontal_flip(int index) const;
    [[nodiscard]] uint8_t cell(int x, int y) const;

    uint8_t operator[](int index);
    // Setters
    void set_horizontal_flip(int index, bool f);
    void insert_map(const game_map room, const bn::point top_left, const bool fliped = false);
    void reset();
private:
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
        if(x < 0 || y < 0 || x >= _width || y >= _height){ // If xy is outside the zone
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
    class Fog;
    void BlockFactory(game_map& map, const bn::point top_left, const uint8_t option, const bool blockFlip);
    bn::point InsertRoom(game_map& map, const bn::point top_left, const uint8_t option, Fog* fog_ptr = NULL);
    void GenerateLevel(game_map& map, Zone& zone, bn::random& randomizer, Fog* fog_ptr = NULL);
}
#endif