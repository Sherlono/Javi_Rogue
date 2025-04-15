#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "jv_actors.h"

#include "bn_sprite_items_stairs.h"

namespace jv{
struct stairs{
    stairs(int x, int y, bn::camera_ptr cam):_sprite(bn::sprite_items::stairs.create_sprite(x, y)), _rect(bn::rect(x, y, 16, 16))
    {
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(2);
    }
    bool climb(jv::Player& player){
        bool isOnStairs = player._rect.intersects(_rect) && bn::keypad::a_pressed();
        if(player.get_state() == State::NORMAL && isOnStairs){
            return true;
        }else{
            return false;
        }
    }
    void set_position(int x, int y){
        _sprite.set_position(x, y);
        _rect.set_position(x, y);
    }
    
    bn::sprite_ptr _sprite;
    bn::rect _rect;
};
}

#endif