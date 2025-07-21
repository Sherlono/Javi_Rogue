#include "jv_stairs.h"

#include "bn_point.h"
#include "bn_keypad.h"

#include "jv_actors.h"
#include "jv_map_classes.h"
#include "jv_blocks_data.h"

namespace jv{
void stairs::set_position(bn::point p){
    _rect.set_position(p);
    bn::point top_left((_rect.position().x()>>3)-2, (_rect.position().y()>>3)-2);
    
    Global::Map().insert_map(game_map(4, 4, (uint8_t*)blocks::data[isOpen ? 33 : 32]), top_left);
}

void stairs::set_open(bool open){
    isOpen = open;
    if(open){
        bn::point top_left((_rect.position().x()>>3)-2, (_rect.position().y()>>3)-2);
        Global::Map().insert_map(game_map(4, 4, (uint8_t*)blocks::data[33]), top_left);
    }else{
        bn::point top_left((_rect.position().x()>>3)-2, (_rect.position().y()>>3)-2);
        Global::Map().insert_map(game_map(4, 4, (uint8_t*)blocks::data[32]), top_left);
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