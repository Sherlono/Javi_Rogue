#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_math.h"

#include "bn_sprite_items_character.h"
#include "bn_sprite_items_cow.h"
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
    Player(unsigned int x, unsigned int y):
    _sprite(bn::sprite_items::character.create_sprite(0 , 0 - 16)),
    _para(x, y, 6, 6),
    _walk(bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2)),
    _speed(bn::fixed(1.0)),
    _prev_dir(2),
    _dir(2)
    {
        _x = x;
        _y = y;
        _sprite.set_bg_priority(1);
    }
    void set_x(bn::fixed x, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, _y - 16);
        }
        _para.set_position(x.integer(), _y.integer());
        _x = x;
    }
    void set_y(bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(_x, y - 16);
        }
        _para.set_position(_x.integer(), y.integer());
        _y = y;
    }
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, y - 16);
        }
        _para.set_position(x.integer(), y.integer());
        _x = x;
        _y = y;
    }

    jv::para get_para(){
        return _para;
    }
    void set_visible(bool visible){
        _sprite.set_visible(visible);
    }
    void walk_update(){
        _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){  // UP
                this->_sprite.set_horizontal_flip(false);
                _walk = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 6){  // RIGHT
                this->_sprite.set_horizontal_flip(false);
                _walk = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                this->_sprite.set_horizontal_flip(false);
                _walk = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){   // LEFT
                this->_sprite.set_horizontal_flip(true);
                _walk = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }
        }
        _prev_dir = _dir;
        _walk.update();
    }
    void wait(){
        _sprite.set_horizontal_flip(_dir == 3);
        _sprite.set_tiles(bn::sprite_items::character.tiles_item().create_tiles(0 + 3*((_dir == 6) || (_dir == 3)) + 6*((_dir == 1 || _dir == 4 || _dir == 7)))); 
    }
    
    void move_player(bn::camera_ptr& cam, bn::vector<jv::para, MAX_BLOCKS>& para_v){
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            // Checking each player corner for colisions
            bool tl = false;
            bool tr = false;
            bool bl = false;
            bool br = false;

            for(unsigned char i = 0; i < para_v.size(); i++){
                bn::fixed x_distance = bn::abs(para_v[i].x() - this->x());
                bn::fixed y_distance = bn::abs(para_v[i].y() - this->y());
                // The following is not replaceable by "intersects()" because it is important to know which corner is contained
                if(x_distance <= 32 && y_distance <= 32){
                    tl = tl + para_v[i].contains_top_left(this->_para);
                    tr = tr + para_v[i].contains_top_right(this->_para);
                    bl = bl + para_v[i].contains_bottom_left(this->_para);
                    br = br + para_v[i].contains_bottom_right(this->_para);
                }
            }
            
            // Move if dir not obstructed
            if(bn::keypad::up_held()){
                if((!tl * !tr) + (!tl * br) + (!tr * bl)){
                    bn::fixed target_y = cam.y() - (_speed + bn::keypad::b_held());
                    cam.set_position(cam.x(), target_y);    // Move camera
                    this->set_position(cam.x(), target_y);  // Move player (position and paralelogram)
                }
            }else if(bn::keypad::down_held()){
                if((!bl * !br) + (!bl * tr) + (!br * tl)){
                    bn::fixed target_y = cam.y() + (_speed + bn::keypad::b_held());
                    cam.set_position(cam.x(), target_y);
                    this->set_position(cam.x(), target_y);
                }
            }
            if(bn::keypad::left_held()){
                if((!tl * !bl) + (!tl * br) + (!bl * tr)){
                    bn::fixed target_x = cam.x() - (_speed + bn::keypad::b_held());
                    cam.set_position(target_x, cam.y());
                    this->set_position(target_x, cam.y());
                }
            }else if(bn::keypad::right_held()){
                if((!tr * !br) + (!tr * bl) + (!br * tl)){
                    bn::fixed target_x = cam.x() + (_speed + bn::keypad::b_held());
                    cam.set_position(target_x, cam.y());
                    this->set_position(target_x, cam.y());    
                }
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
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;
};

class npc: public Actor{
public:
    ~npc(){}
    npc(int x, int y, bn::camera_ptr cam):
    _sprite(bn::sprite_items::cow.create_sprite(x , y - 16)),
    _para(x, y + 6, 12, 12)
    {
        _x = x;
        _y = y;
        _sprite.set_bg_priority(1);
        _sprite.set_camera(cam);
    }
    void set_x(bn::fixed x, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, _y - 16);
        }
        _para.set_position(x.integer(), _y.integer() + 6);
        _x = x;
    }
    void set_y(bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(_x, y - 16);
        }
        _para.set_position(_x.integer(), y.integer() + 6);
        _y = y;
    }
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false){
        if(sprite_follow){
            _sprite.set_position(x, y - 16);
        }
        _para.set_position(x.integer(), y.integer() + 6);
        _x = x;
        _y = y;
    }

    void set_visible(bool visible){
        _sprite.set_visible(visible);
    }
    
    void update(jv::Player& player){
        if(this->y() < player.y()){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }

        if(_para.intersects(player.get_para()) && bn::keypad::a_pressed()){
            jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
        }
    }
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
};
}
#endif