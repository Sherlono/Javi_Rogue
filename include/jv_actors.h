#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_math.h"

#include "bn_sprite_items_character.h"
#include "jv_constants.h"
#include "jv_math.h"

namespace jv{
class Actor{
public:
    virtual ~Actor(){};
    [[nodiscard]] bn::fixed x() const{
        return _x;
    }
    [[nodiscard]] bn::fixed y() const{
        return _y;
    }
    [[nodiscard]] bn::fixed_point xy() const{
        return bn::fixed_point(_x, _y);
    }
    bn::fixed _x, _y;
};


class Player: public Actor{
public:
    ~Player(){};
    Player(int x, int y):
    _sprite(bn::sprite_items::character.create_sprite(0 , 0 - 6)),
    _para(x, y + 6, bn::point(0,0), 6, 6),
    _walk(bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2)),
    _speed(bn::fixed(1.0))
    {
        _x = x;
        _y = y;
        _sprite.set_bg_priority(1);
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

    void set_visible(bool visible){
        _sprite.set_visible(visible);
    }
    void walk_update(){
        _walk.update();
    }
    void wait(){
        _sprite.set_tiles(bn::sprite_items::character.tiles_item().create_tiles(0)); 
    }
    
    void move_player(bn::camera_ptr& cam, bn::vector<jv::para, jv::ct::max_blocks>& para_v){
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            // Checking each player corner for colisions
            bool tl1 = false;
            bool tr1 = false;
            bool bl1 = false;
            bool br1 = false;

            for(unsigned char i = 0; i < para_v.size(); i++){
                bn::fixed x_distance = bn::abs(para_v[i].x() - this->x());
                bn::fixed y_distance = bn::abs(para_v[i].y() -  this->y());
                if(x_distance <= 26 && y_distance <= 26){
                    tl1 = tl1 + para_v[i].contains_top_left(this->_para);
                    tr1 = tr1 + para_v[i].contains_top_right(this->_para);
                    bl1 = bl1 + para_v[i].contains_bottom_left(this->_para);
                    br1 = br1 + para_v[i].contains_bottom_right(this->_para);
                }
            }

            // Move if direction not obstructed
            if(bn::keypad::up_held() && !(tl1 && tr1)){
                cam.set_position(cam.x(), cam.y() - (_speed + bn::keypad::b_held()));
                this->set_position(cam.x(), cam.y() - (_speed + bn::keypad::b_held()));
            }else if(bn::keypad::down_held() && !(bl1 && br1)){
                cam.set_position(cam.x(), cam.y() + (_speed + bn::keypad::b_held()));
                this->set_position(cam.x(), cam.y() + (_speed + bn::keypad::b_held()));
            }
            if(bn::keypad::left_held() && !(tl1 && bl1)){
                cam.set_position(cam.x() - (_speed + bn::keypad::b_held()), cam.y());
                this->set_position(cam.x() - (_speed + bn::keypad::b_held()), cam.y());
            }else if(bn::keypad::right_held() && !(tr1 && br1)){
                cam.set_position(cam.x() + (_speed + bn::keypad::b_held()), cam.y());
                this->set_position(cam.x() + (_speed + bn::keypad::b_held()), cam.y());
            }

            // Animated character
            this->walk_update();
        }else{
            // Insert idle animation here
            this->wait();
        }
    }
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
    bn::sprite_animate_action<4> _walk;
    const bn::fixed _speed;
};
}
#endif