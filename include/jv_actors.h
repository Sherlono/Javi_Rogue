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

struct basic_stats{
    int max_hp , hp, attack, defense;
    bn::fixed speed;
};

class Actor{
public:
    virtual ~Actor(){};
    Actor(bn::sprite_ptr s, bn::fixed x, bn::fixed y, bn::rect r): _sprite(s), _x(x), _y(y), _rect(r){}
    Actor(bn::sprite_ptr s, bn::fixed x, bn::fixed y, bn::rect r, bn::camera_ptr cam): _sprite(s), _x(x), _y(y), _rect(r){ _sprite.set_camera(cam);}
    // Getters
    [[nodiscard]] bn::fixed x() const{ return _x;}
    [[nodiscard]] bn::fixed y() const{ return _y;}
    [[nodiscard]] bn::fixed_point position() const{ return bn::fixed_point(_x, _y);}
    [[nodiscard]] bn::rect rect() const{ return _rect;}
    // Setters
    void set_camera(bn::camera_ptr cam){ _sprite.set_camera(cam);}
    void set_visible(bool visible){ _sprite.set_visible(visible);}

    bn::sprite_ptr _sprite;
    bn::fixed _x, _y;
    bn::rect _rect;
};

/* Directions
 *   4 1 7
 *   3 0 6
 *   5 2 8   */

class Player: public Actor{
public:
    ~Player(){};
    Player(int x, int y, bn::random* ref, game_map* m_r, bn::camera_ptr cam);
    
    // Setters
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow = false);
    void set_position(bn::point point, bool sprite_follow = false);

    void update(bn::camera_ptr cam, bool* isSolid);
protected:
    void animation_update(){
        _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){        // UP
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){  // DOWN
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){                            // LEFT
                _sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 6){                            // RIGHT
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 3, 4, 3, 5);
            }
            _randomizer->update();
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
        _sprite.set_tiles(bn::sprite_items::character.tiles_item().create_tiles(0 + 3*(_dir == 6 || _dir == 3) + 6*(_dir == 1 || _dir == 4 || _dir == 7))); 
    }
    void move(bn::camera_ptr cam, bool isSolid){
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            bool obs_up = true, obs_down = true, obs_left = true, obs_right = true;

            if(isSolid){
                int x = _x.integer()/8, y = _y.integer()/8;
                
                obs_up    = _map_ref->cell(x, y - 1) != 0;
                obs_down  = _map_ref->cell(x, y + 1) != 0;
                obs_left  = _map_ref->cell(x - 1, y) != 0;
                obs_right = _map_ref->cell(x + 1, y) != 0;
            }

            // Move if dir not obstructed
            if(bn::keypad::up_held() && obs_up){
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = cam.y() - (_stats.speed + bn::keypad::b_held())*diagonal;
                cam.set_position(cam.x(), target_y);
                set_position(cam.x(), target_y);
            }else if(bn::keypad::down_held() && obs_down){
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = cam.y() + (_stats.speed + bn::keypad::b_held())*diagonal;
                cam.set_position(cam.x(), target_y);
                set_position(cam.x(), target_y);
            }
            if(bn::keypad::left_held() && obs_left){
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = cam.x() - (_stats.speed + bn::keypad::b_held())*diagonal;
                cam.set_position(target_x, cam.y());
                set_position(target_x, cam.y());
            }else if(bn::keypad::right_held() && obs_right){
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = cam.x() + (_stats.speed + bn::keypad::b_held())*diagonal;
                cam.set_position(target_x, cam.y());
                set_position(target_x, cam.y());
            }

            // Animated character
            animation_update();
        }else{
            // Insert idle animation here
            wait();
        }
    }

private:
    basic_stats _stats;
    bn::sprite_animate_action<4> _animation;
    unsigned char _prev_dir, _dir;

    game_map* _map_ref;
    bn::random* _randomizer;
};

class Enemy: public Actor{
public:
    ~Enemy(){}
    Enemy(int x, int y, bn::random* ref, game_map* m_r, bn::camera_ptr cam);
    // Setters
    void set_position(bn::fixed x, bn::fixed y);
    void set_position(bn::point point);

    void update(jv::Player* player);
protected:
    void animation_update(){
        if(_prev_dir != _dir){
            if(_dir == 1 || _dir == 4 || _dir == 7){        // UP
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 6, 7, 6, 8);
            }else if(_dir == 2 || _dir == 5 || _dir == 8){  // DOWN
                _sprite.set_horizontal_flip(false);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2);
            }else if(_dir == 3){                            // LEFT
                _sprite.set_horizontal_flip(true);
                _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 3, 4, 3, 5);
            }else if(_dir == 6){                            // RIGHT
                _sprite.set_horizontal_flip(false);
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
        _sprite.set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(0 + 3*(_dir == 6 || _dir == 3) + 6*(_dir == 1 || _dir == 4 || _dir == 7))); 
    }

    void move(){
        // Decide direction at random
        if(_idle_time == 0){
            _dir = _randomizer->get_int(16);
            _idle_time++;
        }else if(_idle_time <= 2*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
        
        bool obs_up = true, obs_down = true, obs_left = true, obs_right = true;
        int x = _x.integer()/8, y = _y.integer()/8;
            
        obs_up    = _map_ref->cell(x, y - 1) != 0;
        obs_down  = _map_ref->cell(x, y + 1) != 0;
        obs_left  = _map_ref->cell(x - 1, y) != 0;
        obs_right = _map_ref->cell(x + 1, y) != 0;

        // If direction is valid
        if(_dir != 0 && _dir < 9){
            // Move if dir not obstructed
            if((_dir == 1 || _dir == 4 || _dir == 7) && obs_up){          // UP
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(_dir == 4 || _dir == 7);
                set_position(_x, _y - _stats.speed*diagonal); 
            }else if((_dir == 2 || _dir == 5 || _dir == 8) && obs_down){  // DOWN
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(_dir == 5 || _dir == 8);
                set_position(_x, _y + _stats.speed*diagonal);
            }
            if((_dir == 3 || _dir == 5 || _dir == 4) && obs_left){  // LEFT
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(_dir == 4 || _dir == 5);
                set_position(_x - _stats.speed*diagonal, _y);
            }else if((_dir == 6 || _dir == 7 || _dir == 8) && obs_right){ // RIGHT
                bn::fixed diagonal = 1 + SQRTTWODTWOMONE*(_dir == 7 || _dir == 8);
                set_position(_x + _stats.speed*diagonal, _y);
            }
            
            // Animated character
            animation_update();
        }else{
            // Insert idle animation here
            wait();
        }
    }
    
    void speak(jv::Player* player, const bn::string_view line1, const bn::string_view line2, const bn::string_view line3){
        if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
            jv::Dialog::init(line1, line2, line3);
        }
    }

    /*void hurt(){

    }*/
private:
    basic_stats _stats;
    bn::sprite_animate_action<4> _animation;
    unsigned char _prev_dir, _dir;
    unsigned char _idle_time;

    game_map* _map_ref;
    bn::random* _randomizer;
};

class NPC: public Actor{
public:
    ~NPC(){}
    NPC(int x, int y, bn::camera_ptr cam);
    // Setters
    void set_position(bn::fixed x, bn::fixed y);
    void set_position(bn::point point);

    void update(jv::Player* player);
protected:
    void priority_update(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }
    void speak(jv::Player* player, const bn::string_view line1, const bn::string_view line2, const bn::string_view line3){
        if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
            jv::Dialog::init(line1, line2, line3);
        }
    }
private:

};
    
}

#endif