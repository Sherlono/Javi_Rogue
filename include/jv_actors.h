#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "jv_dialog.h"
#include "jv_constants.h"
#include "bn_sprite_items_character.h"
#include "bn_sprite_items_enemy.h"
#include "bn_sprite_items_cow.h"

namespace jv{
class Actor{
public:
    virtual ~Actor(){};
    // Getters
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
    Player(int x, int y, bn::random* ptr);
    
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
            if(_dir == 1 || _dir == 4 || _dir == 7){  // UP
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 6){  // RIGHT
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){   // LEFT
                this->_sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }
            _random_ptr->update();
        }
        _prev_dir = _dir;
        _animation.update();
    }
    void priority_update(bn::fixed player_y){
        if(this->y() < player_y){
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
            // Move if dir not obstructed
            if(bn::keypad::up_held()){
                bn::fixed target_y = cam.y() - (_speed + bn::keypad::b_held());
                cam.set_position(cam.x(), target_y);    // Move camera
                this->set_position(cam.x(), target_y);  // Move player
            }else if(bn::keypad::down_held()){
                bn::fixed target_y = cam.y() + (_speed + bn::keypad::b_held());
                cam.set_position(cam.x(), target_y);
                this->set_position(cam.x(), target_y);
            }
            if(bn::keypad::left_held()){
                bn::fixed target_x = cam.x() - (_speed + bn::keypad::b_held());
                cam.set_position(target_x, cam.y());
                this->set_position(target_x, cam.y());
            }else if(bn::keypad::right_held()){
                bn::fixed target_x = cam.x() + (_speed + bn::keypad::b_held());
                cam.set_position(target_x, cam.y());
                this->set_position(target_x, cam.y());
            }

            // Animated character
            this->animation_update();
        }else{
            // Insert idle animation here
            this->wait();
        }
    }

private:
    bn::sprite_ptr _sprite;
    bn::sprite_animate_action<4> _animation;
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;
    unsigned char _hp;

    bn::random* _random_ptr;
};

class NPC: public Actor{
public:
    ~NPC(){}
    NPC(int x, int y, bn::camera_ptr& cam);
    // Setters
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false);
    void set_visible(bool visible);
    void update(jv::Player& player);
protected:
    void priority_update(bn::fixed player_y){
        if(this->y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }
    
private:
    bn::sprite_ptr _sprite;
};

class Enemy: public Actor{
public:
    ~Enemy(){}
    Enemy(int x, int y, bn::camera_ptr& cam, bn::random* ptr);
    // Setters
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false);
    void set_visible(bool visible);

    void update(jv::Player& player);
protected:
    void animation_update(){
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){  // UP
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::enemy.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 6){  // RIGHT
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::enemy.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                this->_sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){   // LEFT
                this->_sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::enemy.tiles_item(), 3, 4, 3, 5);
            }
        }
        _prev_dir = _dir;
        _animation.update();
    }
    void priority_update(bn::fixed player_y){
        if(this->y() < player_y){
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
            _dir = _random_ptr->get_int(16);
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
                this->set_position(this->x(), this->y() - _speed, true);  // Move Enemy
            }else if(_dir == 6){  // RIGHT
                this->set_position(this->x() + _speed, this->y(), true);    
            }else if(_dir == 2 || _dir == 5 || _dir == 8){   // DOWN
                this->set_position(this->x(), this->y() + _speed, true);
            }else if(_dir == 3){   // LEFT
                this->set_position(this->x() - _speed, this->y(), true);
            }
            
            // Animated character
            this->animation_update();
        }else{
            // Insert idle animation here
            this->wait();
        }
    }
    /*void hurt(){

    }*/
private:
    bn::sprite_ptr _sprite;
    bn::sprite_animate_action<4> _animation;
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;

    unsigned char _hp;
    unsigned char _idle_time;

    bn::random* _random_ptr;
};

}

#endif