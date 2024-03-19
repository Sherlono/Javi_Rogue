#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_point.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_character.h"
#include "jv_math.h"

namespace jv{
class Actor{
public:
    virtual ~Actor(){};
    [[nodiscard]] bn::fixed x(){
        return _x;
    }
    [[nodiscard]] bn::fixed y(){
        return _y;
    }
    [[nodiscard]] bn::fixed_point xy(){
        return bn::fixed_point(_x, _y);
    }
    bn::fixed _x, _y;
};


class Player: public Actor{
public:
    ~Player(){};
    Player(int x, int y):
    _sprite(bn::sprite_items::character.create_sprite(0 , 0 - 6)),
    _para(x, y + 6, 6, 6),
    _walk(bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 2, 3))
    {
        _x = x;
        _y = y;
        _sprite.set_bg_priority(0);
    }

    [[nodiscard]] jv::para get_para(){
        return this->_para;
    }

    void set_x(bn::fixed x, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, _y - 6);
        }
        _para.set_position(x.round_integer(), _y.round_integer() + 6);
        _x = x;
    }
    void set_y(bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(_x, y - 6);
        }
        _para.set_position(_x.round_integer(), y.round_integer() + 6);
        _y = y;
    }
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, y - 6);
        }
        _para.set_position(x.round_integer(), y.round_integer() + 6);
        _x = x;
        _y = y;
    }

    void walk_update(){
        _walk.update();
    }
    
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
    bn::sprite_animate_action<4> _walk;
};
}
#endif