#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_log.h"
#include "bn_rect.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "jv_math.h"
#include "jv_dialog.h"
#include "jv_constants.h"
#include "jv_environment.h"
#include "bn_sprite_items_character.h"
#include "bn_sprite_items_enemy.h"
#include "bn_sprite_items_cow.h"

namespace jv{
class Actor{
public:
    virtual ~Actor(){};
    Actor(bn::sprite_ptr s, bn::fixed x, bn::fixed y, bn::rect r): _sprite(s), _x(x), _y(y), _rect(r){};
    // Getters
    [[nodiscard]] bn::fixed x() const{
        return _x;
    }
    [[nodiscard]] bn::fixed y() const{
        return _y;
    }
    [[nodiscard]] bn::fixed_point position() const{
        return bn::fixed_point(_x, _y);
    }
    [[nodiscard]] bn::rect rect() const{
        return _rect;
    }
    // Setters
    void set_camera(bn::camera_ptr& cam){
        _sprite.set_camera(cam);
    }
    bn::sprite_ptr _sprite;
    bn::fixed _x, _y;
    bn::rect _rect;
};


class Player: public Actor{
public:
    ~Player(){};
    Player(int x, int y, bn::random& ref, bn::unique_ptr<bg_map>& m_r);
    
    // Setters
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false);
    void set_visible(bool visible);

    void update(bn::camera_ptr& cam);
protected:
    void animation_update(){
        _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){        // UP
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 6){                            // RIGHT
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){  // DOWN
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){                            // LEFT
                _sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }
            _randomizer.update();
        }
        _prev_dir = _dir;
        _animation.update();
    }
    void priority_update(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }
    void wait(){
        _sprite.set_horizontal_flip(_dir == 3);
        _sprite.set_tiles(bn::sprite_items::character.tiles_item().create_tiles(0 + 3*((_dir == 6) || (_dir == 3)) + 6*((_dir == 1 || _dir == 4 || _dir == 7)))); 
    }
    void move(bn::camera_ptr& cam){
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            int int_x = _x.integer() + 128, int_y = _y.integer() + 128;
            int x = bamod(int_x, 256)/8, y = bamod(int_y, 256)/8;
            bool obs_up, obs_down, obs_left, obs_right;
            
            {bn::regular_bg_map_cell_info cell_info(_map_ref->cells[_map_ref->map_item.cell_index(x, y - 1 + 32*(y==0))]);
            obs_up    = cell_info.cell() != 0;}
            {bn::regular_bg_map_cell_info cell_info(_map_ref->cells[_map_ref->map_item.cell_index(x, y + 1 - 32*(y==31))]);
            obs_down    = cell_info.cell() != 0;}
            {bn::regular_bg_map_cell_info cell_info(_map_ref->cells[_map_ref->map_item.cell_index(x - 1 + 32*(x==0), y)]);
            obs_left    = cell_info.cell() != 0;}
            {bn::regular_bg_map_cell_info cell_info(_map_ref->cells[_map_ref->map_item.cell_index(x + 1 - 32*(x==31), y)]);
            obs_right    = cell_info.cell() != 0;}

            // Move if dir not obstructed
            if(bn::keypad::up_held() && obs_up){
                bn::fixed target_y = cam.y() - (_speed + bn::keypad::b_held());
                cam.set_position(cam.x(), target_y);    // Move camera
                set_position(cam.x(), target_y);  // Move player
                _rect.set_position(cam.x().integer(), target_y.integer());
            }else if(bn::keypad::down_held() && obs_down){
                bn::fixed target_y = cam.y() + (_speed + bn::keypad::b_held());
                cam.set_position(cam.x(), target_y);
                set_position(cam.x(), target_y);
                _rect.set_position(cam.x().integer(), target_y.integer());
            }
            if(bn::keypad::left_held() && obs_left){
                bn::fixed target_x = cam.x() - (_speed + bn::keypad::b_held());
                cam.set_position(target_x, cam.y());
                set_position(target_x, cam.y());
                _rect.set_position(target_x.integer(), cam.x().integer());
            }else if(bn::keypad::right_held() && obs_right){
                bn::fixed target_x = cam.x() + (_speed + bn::keypad::b_held());
                cam.set_position(target_x, cam.y());
                set_position(target_x, cam.y());
                _rect.set_position(target_x.integer(), cam.x().integer());
            }

            // Animated character
            animation_update();
        }else{
            // Insert idle animation here
            wait();
        }
    }

private:
    bn::sprite_animate_action<4> _animation;
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;
    unsigned char _hp;

    bn::unique_ptr<bg_map>& _map_ref;
    bn::random& _randomizer;
};

class NPC: public Actor{
public:
    ~NPC(){}
    NPC(int x, int y);
    // Setters
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false);
    void set_visible(bool visible);
    void update(jv::Player& player);
protected:
    void priority_update(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }
    
private:

};

class Enemy: public Actor{
public:
    ~Enemy(){}
    Enemy(int x, int y, bn::random& ref);
    // Setters
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y);
    void set_visible(bool visible);

    void update(jv::Player& player);
protected:
    void animation_update(){
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){  // UP
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 6){  // RIGHT
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){   // LEFT
                _sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 3, 4, 3, 5);
            }
        }
        _prev_dir = _dir;
        _animation.update();
    }

    void priority_update(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }

    void wait(){
        _sprite.set_horizontal_flip(_dir == 3);
        _sprite.set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(0 + 3*((_dir == 6) || (_dir == 3)) + 6*((_dir == 1 || _dir == 4 || _dir == 7)))); 
    }

    void move(){
        // Decide direction at random
        if(_idle_time == 0){
            _dir = _randomizer.get_int(16);
            _idle_time++;
        }else if(_idle_time <= 2*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
        
        // If direction is valid
        if(_dir != 0 && _dir < 9){
            // Move if dir not obstructed
            if(_dir == 1 || _dir == 4 || _dir == 7){  // UP
                set_position(x(), y() - _speed);  // Move Enemy
            }else if(_dir == 6){  // RIGHT
                set_position(x() + _speed, y());    
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                set_position(x(), y() + _speed);
            }else if(_dir == 3){   // LEFT
                set_position(x() - _speed, y());
            }
            
            // Animated character
            animation_update();
        }else{
            // Insert idle animation here
            wait();
        }
    }
    /*void hurt(){

    }*/
private:
    bn::sprite_animate_action<4> _animation;
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;

    unsigned char _hp;
    unsigned char _idle_time;

    bn::random& _randomizer;
};

}

#endif