#include "jv_stairs.h"

#include "bn_log.h"
#include "bn_point.h"
#include "bn_keypad.h"

#include "jv_actors.h"
#include "jv_map_classes.h"
#include "jv_blocks_data.h"

#include "bn_regular_bg_tiles_items_fortress_tiles.h"
#include "bn_regular_bg_tiles_items_fortress_stairs.h"
#include "bn_regular_bg_tiles_items_jungle_tiles.h"
#include "bn_regular_bg_tiles_items_jungle_stairs.h"

namespace jv{
void stairs::set_position(bn::point p){
    _rect.set_position(p);
    bn::point top_left((_rect.position().x()>>3)-2, (_rect.position().y()>>3)-2);
    
    Global::Map().insert_data(4, 4, jv::blocks::get_block(32), top_left);
}

void stairs::set_open(const bn::regular_bg_tiles_ptr& t_ptr, bool open){
    bn::span<const bn::tile> tiles;
    isOpen = open;
    if(open){
        switch (Global::environment_id) {
            case Global::Environments::Fortress:{
                tiles = bn::regular_bg_tiles_items::fortress_stairs.tiles_ref();
                break;}
            case Global::Environments::Jungle:{
                tiles = bn::regular_bg_tiles_items::jungle_stairs.tiles_ref();
                break;}
            default:
                BN_ERROR("Missing stairs graphics.");
                break;
        }
    }else{
        switch (Global::environment_id) {
            case Global::Environments::Fortress:{
                tiles = bn::regular_bg_tiles_items::fortress_tiles.tiles_ref();
                break;}
            case Global::Environments::Jungle:{
                tiles = bn::regular_bg_tiles_items::jungle_tiles.tiles_ref();
                break;}
            default:
                BN_ERROR("Missing stairs graphics.");
                break;
        }
    }

    bn::regular_bg_tiles_ptr tiles_ptr = t_ptr;
    int current_graphics_index = open ? 0 : stairs_tiles_begin;
    int tile_index = stairs_tiles_begin;
    
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            const bn::tile& tile = tiles[current_graphics_index];
            
            tiles_ptr.overwrite_tile(tile_index, tile);
            current_graphics_index++;
            tile_index++;
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