#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_log.h"
#include "bn_rect.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_blending_actions.h"

#include "jv_math.h"
#include "jv_dialog.h"
#include "jv_constants.h"
#include "jv_environment.h"
#include "bn_sprite_items_character.h"
#include "bn_sprite_items_enemy.h"
#include "bn_sprite_items_cow.h"

namespace jv{

struct basic_stats{
    basic_stats(int maxhp, int att, int def, bn::fixed spe): max_hp(maxhp), hp(maxhp), attack(att), defense(def), speed(spe){}
    int max_hp, hp, attack, defense;
    bn::fixed speed;
};

enum Direction { NEUTRAL, NORTH, SOUTH, WEST, NORTHWEST, SOUTHWEST, EAST, NORTHEAST, SOUTHEAST};
enum State { NORMAL, ATTACKING, HURTING, DEAD};

class Actor{
public:
    virtual ~Actor(){};
    // Constructor
    Actor(int x, int y, bn::sprite_ptr s, bn::camera_ptr cam, bn::sprite_animate_action<4> anim, bn::rect r): _x(x), _y(y), _sprite(s), _animation(anim), _rect(r)
    {
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(1);
    }
    // Getters
    [[nodiscard]] bn::fixed x() const{ return _x;}
    [[nodiscard]] bn::fixed y() const{ return _y;}
    [[nodiscard]] bn::fixed_point position() const{ return bn::fixed_point(_x, _y);}
    [[nodiscard]] bn::rect rect() const{ return _rect;}

    // Setters
    void set_position(bn::fixed x, bn::fixed y, bn::fixed y_offset = 8.0){
        _sprite.set_position(x, y - y_offset);
        _x = x;
        _y = y;
        _rect.set_position(x.integer(), y.integer());
    }
    
    void set_position(bn::point point, bn::fixed y_offset = 8.0){
        _sprite.set_position(point.x(), point.y() - y_offset);
        _x = point.x();
        _y = point.y();
        _rect.set_position(point.x(), point.y());
    }
    
    void set_camera(bn::camera_ptr cam){ _sprite.set_camera(cam);}
    void remove_camera(){ _sprite.remove_camera();}
    void set_visible(bool visible){ _sprite.set_visible(visible);}
    void set_blending_enabled(bool isBlend){ _sprite.set_blending_enabled(isBlend);}


    bn::fixed _x, _y;
    bn::sprite_ptr _sprite;
    bn::sprite_animate_action<4> _animation;
    bn::rect _rect;
};

class Player: public Actor{
public:
    ~Player(){};
    // Constructor
    Player(int x, int y, bn::sprite_ptr s, const bn::sprite_tiles_item &s_item, bn::camera_ptr c, bn::random* random_ref, game_map* m_r):
        Actor(x, y, s, c,
              bn::create_sprite_animate_action_forever(s, 4, s_item, frames::w_do[0], frames::w_do[1], frames::w_do[2], frames::w_do[3]),
              bn::rect(x, y, 10, 10)),
        _stats(basic_stats(5, 1, 1, bn::fixed(1.5))),
        _state(State::NORMAL),
        _hitbox(bn::rect(x, y, 10, 10)),
        cam(c),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _prev_dir(2),
        _dir(2),
        _map_ref(m_r),
        _randomizer(random_ref)
        {
            _sprite.set_position(x, y - 8);
        }
    
    // Setters
    void set_state(int s){ _state = s;}
    void reset(){
        _dir = jv::SOUTH;
        insert_animation(frames::w_up, frames::w_ho, frames::w_do);
        _animation.update();
    }

    // Getters
    [[nodiscard]] bool is_attacking() { return _state == State::ATTACKING;}
    [[nodiscard]] uchar_t get_state() { return _state;}
    [[nodiscard]] int get_attack() { return _stats.attack;}
    [[nodiscard]] int get_defense() { return _stats.defense;}
    [[nodiscard]] int get_maxhp() { return _stats.max_hp;}
    [[nodiscard]] int get_hp() { return _stats.hp;}
    [[nodiscard]] bn::rect get_hitbox() { return _hitbox;}
    [[nodiscard]] int* get_hp_ptr(){ return &_stats.hp;}
    [[nodiscard]] int* get_maxhp_ptr(){ return &_stats.max_hp;}

    void update(bool noClip);
    
    void animation_update(){
        if(_prev_dir != _dir){
            insert_animation(frames::w_up, frames::w_ho, frames::w_do);
        }
        _prev_dir = _dir;
    }
    
    void move(bool noClip = false){
        if(!_attack_cooldown){
            if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
                _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();
                bool obs_up = true, obs_down = true, obs_left = true, obs_right = true;

                if(!noClip){
                    int x = _x.integer()/8, y = (_y.integer() + 4)/8;
                    obs_up    = _map_ref->cell(x, y - 1) > 0 && _map_ref->cell(x, y - 1) < WT_COUNT;
                    obs_down  = _map_ref->cell(x, y + 1) > 0 && _map_ref->cell(x, y + 1) < WT_COUNT;
                    obs_left  = _map_ref->cell(x - 1, y) > 0 && _map_ref->cell(x - 1, y) < WT_COUNT;
                    obs_right = _map_ref->cell(x + 1, y) > 0 && _map_ref->cell(x + 1, y) < WT_COUNT;
                }

                // Move if dir not obstructed
                if(bn::keypad::up_held() && obs_up){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                    bn::fixed target_y = cam.y() - _stats.speed*diagonal;
                    cam.set_position(cam.x(), target_y);
                    set_position(cam.x(), target_y);
                }else if(bn::keypad::down_held() && obs_down){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                    bn::fixed target_y = cam.y() + _stats.speed*diagonal;
                    cam.set_position(cam.x(), target_y);
                    set_position(cam.x(), target_y);
                }
                if(bn::keypad::left_held() && obs_left){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                    bn::fixed target_x = cam.x() - _stats.speed*diagonal;
                    cam.set_position(target_x, cam.y());
                    set_position(target_x, cam.y());
                }else if(bn::keypad::right_held() && obs_right){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                    bn::fixed target_x = cam.x() + _stats.speed*diagonal;
                    cam.set_position(target_x, cam.y());
                    set_position(target_x, cam.y());
                }
            }
        
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!_attack_cooldown && _state != State::HURTING){
            _attack_cooldown = 20;
            insert_animation(frames::a_up, frames::a_ho, frames::a_do);
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
    }
    
    void got_hit(int damage){
        _state = State::HURTING;
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        _stats.hp -= damage;
        if(_stats.hp <= 0){
            _state = State::DEAD;
            _sprite.set_horizontal_flip(false);
            _sprite.set_tiles(bn::sprite_items::character.tiles_item().create_tiles(24));
        }else{
            _sprite.set_horizontal_flip(_dir == jv::WEST);
            _animation = bn::create_sprite_animate_action_once(_sprite, 8, bn::sprite_items::character.tiles_item(),
                                                               frames::hurt[0], frames::hurt[1], frames::hurt[2], frames::hurt[3]);
        }
    }
    
private:

    void insert_animation(cuchar_t* up, cuchar_t* horizontal, cuchar_t* down){
        if(_dir == jv::NORTH || _dir == jv::NORTHWEST || _dir == jv::NORTHEAST){        // UP
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), up[0], up[1], up[2], up[3]);
            _hitbox.set_position(_x.integer(), (_y - 16).integer());
        }else if(_dir == jv::SOUTH || _dir == jv::SOUTHWEST || _dir == jv::SOUTHEAST){  // DOWN
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), down[0], down[1], down[2], down[3]);
            _hitbox.set_position(_x.integer(), (_y + 16).integer());
        }else if(_dir == jv::WEST){                            // LEFT
            _sprite.set_horizontal_flip(true);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), horizontal[0], horizontal[1], horizontal[2], horizontal[3]);
            _hitbox.set_position((_x - 16).integer(), _y.integer());
        }else if(_dir == jv::EAST){                            // RIGHT
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), horizontal[0], horizontal[1], horizontal[2], horizontal[3]);
            _hitbox.set_position((_x + 16).integer(), _y.integer());
        }else{
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), frames::idle[0], frames::idle[1], frames::idle[2], frames::idle[3]);
        }
    }

    basic_stats _stats;
    uchar_t _state;
    bn::rect _hitbox;
    bn::camera_ptr cam;
    int _prev_attack_cooldown, _attack_cooldown;
    uchar_t _prev_dir, _dir;

    game_map* _map_ref;
    bn::random* _randomizer;
};

class Enemy: public Actor{
public:
    ~Enemy(){}
    // Constructor
    Enemy(int x, int y, bn::sprite_ptr s, const bn::sprite_tiles_item &s_item, bn::camera_ptr cam, bn::random* random_ref, game_map* m_r):
        Actor(x, y, s, cam,
              bn::create_sprite_animate_action_forever(s, 4, s_item, frames::w_do[0], frames::w_do[1], frames::w_do[2], frames::w_do[3]),
              bn::rect(x, y, 10, 10)),
        _stats(basic_stats(3, 1, 1, bn::fixed(0.4))),
        _state(State::NORMAL),
        _hitbox(bn::rect(x, y, 10, 10)),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _prev_dir(2),
        _dir(2),
        _idle_time(0),
        _map_ref(m_r),
        _randomizer(random_ref)
        {
            _sprite.set_position(x, y - 8);
        }
    // Setters
    void set_state(int s){ _state = s;}

    // Getters
    [[nodiscard]] bool is_attacking(){ return _state == State::ATTACKING;}
    [[nodiscard]] uchar_t get_state() { return _state;}
    [[nodiscard]] int get_attack() { return _stats.attack;}
    [[nodiscard]] int get_defense() { return _stats.defense;}
    [[nodiscard]] int get_maxhp() { return _stats.max_hp;}
    [[nodiscard]] int get_hp() { return _stats.hp;}
    [[nodiscard]] bn::rect get_hitbox() { return _hitbox;}

    void update(jv::Player* player);
    
    void animation_update(){
        if(_prev_dir != _dir){
            insert_animation(frames::w_up, frames::w_ho, frames::w_do);
        }
        _prev_dir = _dir;
    }

    void y_priority(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }

    void move(){
        // Decide direction at random
        if(!_attack_cooldown){
            if(_idle_time == 0){
                _dir = _randomizer->get_int(16);
                _idle_time++;
            }else if(_idle_time <= 2*60 + _dir*2){
                _idle_time++;
            }else{
                _idle_time = 0;
            }
            bool obs_up = true, obs_down = true, obs_left = true, obs_right = true;
            int x = _x.integer()/8, y = (_y.integer() + 4)/8;
                
            obs_up    = _map_ref->cell(x, y - 1) > 0 && _map_ref->cell(x, y - 1) < WT_COUNT;
            obs_down  = _map_ref->cell(x, y + 1) > 0 && _map_ref->cell(x, y + 1) < WT_COUNT;
            obs_left  = _map_ref->cell(x - 1, y) > 0 && _map_ref->cell(x - 1, y) < WT_COUNT;
            obs_right = _map_ref->cell(x + 1, y) > 0 && _map_ref->cell(x + 1, y) < WT_COUNT;

            // If direction is valid
            if(_dir != jv::NEUTRAL && _dir < 9){
                // Move if dir not obstructed
                if((_dir == jv::NORTH || _dir == jv::NORTHWEST || _dir == jv::NORTHEAST) && obs_up){          // UP
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == jv::NORTHWEST || _dir == jv::NORTHEAST);
                    set_position(_x, _y - _stats.speed*diagonal); 
                }else if((_dir == jv::SOUTH || _dir == jv::SOUTHWEST || _dir == jv::SOUTHEAST) && obs_down){  // DOWN
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == jv::SOUTHWEST || _dir == jv::SOUTHEAST);
                    set_position(_x, _y + _stats.speed*diagonal);
                }
                if((_dir == jv::WEST || _dir == jv::NORTHWEST || _dir == jv::SOUTHWEST) && obs_left){  // LEFT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == jv::NORTHWEST || _dir == jv::SOUTHWEST);
                    set_position(_x - _stats.speed*diagonal, _y);
                }else if((_dir == jv::EAST || _dir == jv::NORTHEAST || _dir == jv::SOUTHEAST) && obs_right){ // RIGHT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == jv::NORTHEAST || _dir == jv::SOUTHEAST);
                    set_position(_x + _stats.speed*diagonal, _y);
                }
            }
            
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!_attack_cooldown){
            _idle_time = 0;
            _attack_cooldown = 20;
            insert_animation(frames::a_up, frames::a_ho, frames::a_do);
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
    }
    
    void got_hit(int damage){
        _state = State::HURTING;
        _dir = 0;
        _stats.hp -= damage;
        if(_stats.hp <= 0){
            _state = State::DEAD;
            _sprite.set_horizontal_flip(false);
            _sprite.set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(24));
        }else{
            _sprite.set_horizontal_flip(_dir == jv::WEST);
            _animation = bn::create_sprite_animate_action_once(_sprite, 8, bn::sprite_items::enemy.tiles_item(),
                                                               frames::hurt[0], frames::hurt[1], frames::hurt[2], frames::hurt[3]);
        }
    }
    

private:
    void insert_animation(cuchar_t* up, cuchar_t* horizontal, cuchar_t* down){
        if(_dir == jv::NORTH || _dir == jv::NORTHWEST || _dir == jv::NORTHEAST){        // UP
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), up[0], up[1], up[2], up[3]);
            _hitbox.set_position(_x.integer(), (_y - 16).integer());
        }else if(_dir == jv::SOUTH || _dir == jv::SOUTHWEST || _dir == jv::SOUTHEAST){  // DOWN
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), down[0], down[1], down[2], down[3]);
            _hitbox.set_position(_x.integer(), (_y + 16).integer());
        }else if(_dir == jv::WEST){                            // LEFT
            _sprite.set_horizontal_flip(true);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), horizontal[0], horizontal[1], horizontal[2], horizontal[3]);
            _hitbox.set_position((_x - 16).integer(), _y.integer());
        }else if(_dir == jv::EAST){                            // RIGHT
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), horizontal[0], horizontal[1], horizontal[2], horizontal[3]);
            _hitbox.set_position((_x + 16).integer(), _y.integer());
        }else{
            _sprite.set_horizontal_flip(false);
            _animation = bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), frames::idle[0], frames::idle[1], frames::idle[2], frames::idle[3]);
        }
    }

    basic_stats _stats;
    uchar_t _state;
    bn::rect _hitbox;
    int _prev_attack_cooldown, _attack_cooldown;
    uchar_t _prev_dir, _dir;
    uchar_t _idle_time;

    game_map* _map_ref;
    bn::random* _randomizer;
};

class NPC: public Actor{
public:
    ~NPC(){}
    // Constructor
    NPC(int x, int y, bn::sprite_ptr s, const bn::sprite_tiles_item &s_item, bn::camera_ptr cam):
        Actor(x, y, s, cam,
              bn::create_sprite_animate_action_forever(s, 4, s_item, 0, 0, 0, 0),
              bn::rect(x, y + 8, 20, 20))
    {
        _sprite.set_position(x, y - 8);
    }
    // Setters

    void update(jv::Player* player);
    
    void y_priority(bn::fixed player_y){
        if(y() < player_y){
            _sprite.set_z_order(1);
        }else{
            _sprite.set_z_order(-1);
        }
    }
private:

};
    
}

#endif