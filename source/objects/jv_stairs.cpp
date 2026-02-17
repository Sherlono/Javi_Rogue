#include "jv_stairs.h"

#include "bn_point.h"
#include "bn_keypad.h"

#include "jv_actors.h"
#include "jv_map_classes.h"
#include "jv_blocks_data.h"

#include "bn_regular_bg_tiles_items_fortress_tiles.h"
#include "bn_regular_bg_tiles_items_fortress_stairs.h"

namespace jv{
void stairs::set_position(bn::point p){
    _rect.set_position(p);
    bn::point top_left((_rect.position().x()>>3)-2, (_rect.position().y()>>3)-2);
    
    Global::Map().insert_data(4, 4, (GameMap::cell_type*)blocks::data[isOpen ? 33 : 32], top_left);
}

void stairs::set_open(bool open){
    isOpen = open;
    int current_graphics_index = 0;
    int tile_index = 41;
    if(open){
        const bn::span<const bn::tile>& stairs_tiles = bn::regular_bg_tiles_items::fortress_stairs.tiles_ref();
        for(int y = 0; y < 4; y++){
            for(int x = 0; x < 4; x++){
                const bn::tile& tile = stairs_tiles[current_graphics_index];
                
                _tiles_ptr.overwrite_tile(tile_index, tile);
                current_graphics_index++;
                tile_index++;
            }
        }
    }else{
        const bn::span<const bn::tile>& stairs_tiles = bn::regular_bg_tiles_items::fortress_tiles.tiles_ref();
        for(int y = 0; y < 4; y++){
            for(int x = 0; x < 4; x++){
                const bn::tile& tile = stairs_tiles[current_graphics_index];
                
                _tiles_ptr.overwrite_tile(tile_index, tile);
                current_graphics_index++;
                tile_index++;
            }
        }
    }
}

bool stairs::climb(){
    bool isOnStairs = Global::Player().rect().intersects(_rect) && bn::keypad::a_pressed();
    if(isOpen && Global::Player().get_state() == Actor::State::NORMAL && isOnStairs){
        return true;
    }else{
        return false;
    }
}
}