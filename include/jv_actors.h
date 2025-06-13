#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_rect.h"
#include "bn_assert.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_sprite_builder.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "jv_math.h"
#include "jv_stairs.h"
#include "jv_dialog.h"
#include "jv_constants.h"
#include "jv_environment.h"

#include "bn_sprite_items_cow.h"
#include "bn_sprite_items_bad_cat.h"
#include "bn_sprite_items_good_cat.h"
#include "bn_sprite_items_pale_tongue.h"
#include "bn_sprite_items_pale_finger.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

namespace jv{
struct stairs;

class Actor{
public:
    virtual ~Actor(){ reset_graphics();};
    // Constructor
    Actor(bn::rect r): _dir(Direction::SOUTH), _rect(r){}

    enum Direction { NEUTRAL, NORTH, SOUTH, WEST, NORTHWEST, SOUTHWEST, EAST, NORTHEAST, SOUTHEAST};
    enum State { NORMAL, ATTACKING, HURTING, DEAD};
    
    struct basic_stats{
        constexpr basic_stats(const uint8_t att, const uint8_t def, const short maxhp, const bn::fixed spe):
            attack(att), defense(def), max_hp(maxhp), speed(spe){}
        uint8_t attack, defense;
        short max_hp;
        bn::fixed speed;
    };
    
    // Getters
    [[nodiscard]] int int_x() const{ return _rect.x();}
    [[nodiscard]] int int_y() const{ return _rect.y();}
    [[nodiscard]] bn::fixed x() const{ return _sprite->x();}
    [[nodiscard]] bn::fixed y() const{ return _sprite->y();}
    [[nodiscard]] bn::fixed_point position() const{ return _sprite->position();}
    [[nodiscard]] bn::rect rect() const{ return _rect;}
    [[nodiscard]] bn::sprite_ptr sprite() { return _sprite.value();}
    [[nodiscard]] bn::sprite_animate_action<MAX_FRAMES> animation() { return _animation.value();}
    [[nodiscard]] bool in_range(int x, int y, const int distance){
        return x - int_x() <= distance && x - int_x() >= -distance && y - int_y() <= distance && y - int_y() >= -distance;
    }
    [[nodiscard]] bool obstacle(int x, int y, const uint8_t direction, game_map& map){
        switch(direction){
            case Direction::NORTH:
                return map.cell(x, y - 1) > 0 && map.cell(x, y - 1) < WTILES_COUNT;
                break;
            case Direction::SOUTH:
                return map.cell(x, y + 1) > 0 && map.cell(x, y + 1) < WTILES_COUNT;
                break;
            case Direction::WEST:
                return map.cell(x - 1, y) > 0 && map.cell(x - 1, y) < WTILES_COUNT;
                break;
            case Direction::EAST:
                return map.cell(x + 1, y) > 0 && map.cell(x + 1, y) < WTILES_COUNT;
                break;
            default:
                BN_ASSERT(false, "Invalid direction", direction);
                return false;
                break;
        }
    }

    // Setters
    void set_position(bn::fixed x, bn::fixed y, uint16_t y_offset = 8){
        _sprite->set_position(x, y - y_offset);
        _rect.set_position(x.integer(), y.integer());
    }
    void set_position(bn::fixed_point point, uint16_t y_offset = 8){
        _sprite->set_position(point.x(), point.y() - y_offset);
        _rect.set_position(point.x().integer(), point.y().integer());
    }
    void set_camera(bn::camera_ptr& cam){ _sprite->set_camera(cam);}
    void remove_camera(){ _sprite->remove_camera();}
    void set_visible(bool visible){ _sprite->set_visible(visible);}
    void set_blending_enabled(bool isBlend){ _sprite->set_blending_enabled(isBlend);}

    void look_at(bn::fixed_point& xyVector, bn::fixed& abs_x, bn::fixed& abs_y){
        if(xyVector.y() < -0.5){
            if(abs_y > abs_x){
                _dir = Direction::NORTH;
            }else if(xyVector.x() > 0){
                _dir = Direction::NORTHEAST;
            }else{
                _dir = Direction::NORTHWEST;
            }
        }else if(xyVector.y() > 0.5){
            if(abs_y > abs_x){
                _dir = Direction::SOUTH;
            }else if(xyVector.x() > 0){
                _dir = Direction::SOUTHEAST;
            }else{
                _dir = Direction::SOUTHWEST;
            }
        }else{
            if(xyVector.x() > 0){
                _dir = Direction::EAST;
            }else{
                _dir = Direction::WEST;
            }
        }
    }

    void set_animation(const uint8_t option, const bn::sprite_tiles_item& tiles, const uint8_t wait_frames = 4){
        if(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST){        // UP
            _sprite->set_horizontal_flip(false);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames::WalkAttack_up[option]);
        }else if(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST){  // DOWN
            _sprite->set_horizontal_flip(false);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames::WalkAttack_do[option]);
        }else if(_dir == Direction::WEST){                            // LEFT
            _sprite->set_horizontal_flip(true);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames::WalkAttack_ho[option]);
        }else if(_dir == Direction::EAST){                            // RIGHT
            _sprite->set_horizontal_flip(false);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames::WalkAttack_ho[option]);
        }else{
            _sprite->set_horizontal_flip(false);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames::idle);
        }
    }

    void reset_graphics(){
        _sprite.reset();
        _animation.reset();
    }

protected:
    uint8_t _dir;
    bn::rect _rect;
    bn::optional<bn::sprite_ptr> _sprite;
    bn::optional<bn::sprite_animate_action<MAX_FRAMES>> _animation;
};

class Player: public Actor{
public:
    ~Player(){ reset_graphics();};
    // Constructor
    Player(bn::point position, bn::camera_ptr& camera):
        Actor(bn::rect(position.x(), position.y(), 16, 16)),
        _interact_token(true),
        _stats(basic_stats(1, 1, 5, bn::fixed(1.5))),
        _state(State::NORMAL),
        _hitbox(bn::rect(position.x(), position.y(), 10, 10)),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _prev_dir(Direction::SOUTH)
        {
            _hp = _stats.max_hp;
            bn::sprite_builder builder(bn::sprite_items::good_cat);
            builder.set_position(position.x(), position.y() - 8);
            builder.set_camera(camera);
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build();
            _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), 4, bn::sprite_items::good_cat.tiles_item(), frames::WalkAttack_do[frames::Walk]);
        }
    
    // Getters
    [[nodiscard]] bool alive() { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking() { return bool(_attack_cooldown);}
    [[nodiscard]] inline bool attack_ended() { return !is_attacking() && _prev_attack_cooldown != _attack_cooldown;}

    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] int get_attack() const { return _stats.attack;}
    [[nodiscard]] int get_defense() const { return _stats.defense;}
    [[nodiscard]] int get_maxhp() const { return _stats.max_hp;}
    [[nodiscard]] int get_hp() const { return _hp;}
    [[nodiscard]] bn::rect get_hitbox() { return _hitbox;}

    [[nodiscard]] short* get_hp_ptr() { return &_hp;}
    [[nodiscard]] short* get_maxhp_ptr() { return &_stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void reset(){
        _prev_dir = Direction::NEUTRAL;
        _dir = Direction::SOUTH;
        _hitbox.set_position(this->int_x(), this->int_y());
        set_animation(frames::Walk, bn::sprite_items::good_cat.tiles_item());
        _animation->update();
    }

    void update(bn::camera_ptr& cam, game_map& map, bool noClip);

    void heal(int h){
        _hp = bn::min(_hp + h, int(_stats.max_hp));
    }

    void got_hit(int damage, bool ignoreDef = false){
        _state = State::HURTING;
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        if(ignoreDef){
            _hp -= damage;
        }else{
            _hp -= damage/_stats.defense;
        }
        if(_hp <= 0){
            _state = State::DEAD;
            _sprite->set_horizontal_flip(false);
            _sprite->set_tiles(bn::sprite_items::good_cat.tiles_item().create_tiles(24));
        }else{
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::good_cat.tiles_item(), frames::hurt);
        }
    }
    
    bool _interact_token;

private:
    void animation_update(){
        if(_prev_dir != _dir){
            set_animation(frames::Walk, bn::sprite_items::good_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
    
    void move(game_map& map, bool noClip = false){
        if(!_attack_cooldown){
            if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
                _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();

                int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

                // Move if dir not obstructed
                if(bn::keypad::up_held() && (noClip || obstacle(x, y, NORTH, map))){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                    bn::fixed target_y = this->y() - _stats.speed*diagonal;
                    set_position(this->x(), target_y + 8, 8);
                }else if(bn::keypad::down_held() && (noClip || obstacle(x, y, SOUTH, map))){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                    bn::fixed target_y = this->y() + _stats.speed*diagonal;
                    set_position(this->x(), target_y + 8, 8);
                }
                if(bn::keypad::left_held() && (noClip || obstacle(x, y, WEST, map))){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                    bn::fixed target_x = this->x() - _stats.speed*diagonal;
                    set_position(target_x, this->y() + 8, 8);
                }else if(bn::keypad::right_held() && (noClip || obstacle(x, y, EAST, map))){
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                    bn::fixed target_x = this->x() + _stats.speed*diagonal;
                    set_position(target_x, this->y() + 8, 8);
                }
                _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                                    this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
            }
    
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!is_attacking() && _state != State::HURTING){
            _attack_cooldown = 20;
            set_animation(frames::Attack, bn::sprite_items::good_cat.tiles_item());
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
        if(attack_ended()){
            set_animation(frames::Walk, bn::sprite_items::good_cat.tiles_item());
        }
    }
    
    short _hp;
    basic_stats _stats;
    uint8_t _state;
    bn::rect _hitbox;
    int _prev_attack_cooldown, _attack_cooldown;
    uint8_t _prev_dir;
};

class Enemy: public Actor{
public:
    ~Enemy(){ reset_graphics();}
    Enemy(bn::point position):
        Actor(bn::rect(position.x(), position.y(), 16, 16)),
        _state(State::NORMAL),
        _hitbox(bn::rect(position.x(), position.y(), 10, 10)),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _prev_dir(Direction::SOUTH),
        _idle_time(0) {}

    // Getters
    [[nodiscard]] bool alive() const { return _state != State::DEAD;}
    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] short get_hp() const { return hp;}
    [[nodiscard]] bn::rect get_hitbox() const { return _hitbox;}
    [[nodiscard]] inline bool on_screen(bn::camera_ptr& cam, uint8_t halfWidth = 16, uint8_t halfHeight = 16) const {
        uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
        bool up = this->int_y() > cam.y() - y_offset, down = this->int_y() < cam.y() + y_offset;
        bool left = this->int_x() > cam.x() - x_offset, right = this->int_x() < cam.x() + x_offset;
        return left && right && up && down;
    }
    [[nodiscard]] bool is_attacking() const { return bool(_attack_cooldown);}
    [[nodiscard]] inline bool attack_ended() {
        return !is_attacking() && _prev_attack_cooldown != _attack_cooldown;
    }

    // Functionality
    virtual void update(jv::Player& player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul) = 0;

protected:
    short hp;
    uint8_t _state;
    bn::rect _hitbox;
    int8_t _prev_attack_cooldown, _attack_cooldown;
    uint8_t _prev_dir;
    uint8_t _idle_time;
};

class BadCat: public Enemy{
public:
    ~BadCat(){ reset_graphics();}
    // Constructor
    BadCat(bn::point position, bn::camera_ptr cam):
        Enemy(position)
        {
            hp = _stats.max_hp;
            bool isOnScreen = on_screen(cam);
            if(isOnScreen){
                bn::sprite_builder builder(bn::sprite_items::bad_cat);
                builder.set_position(position.x(), position.y() - 8);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), 4, bn::sprite_items::bad_cat.tiles_item(), frames::WalkAttack_do[frames::Walk]);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update(jv::Player& player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul) override;
    
    void got_hit(int damage){
        _state = State::HURTING;
        _prev_dir = 0;
        _dir = 0;
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::bad_cat.tiles_item(), frames::hurt);
        }
    }
    
private:
    void animation_update(){
        if(_prev_dir != _dir){
            set_animation(frames::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
    
    void move(game_map& map, jv::Player& player, bn::random& randomizer){
        // Decide direction at random
        if(!_attack_cooldown){
            bn::fixed_point xyVector = jv::normalize(player.position() - position());
            bn::fixed abs_x = bn::abs(xyVector.x()), abs_y = bn::abs(xyVector.y());
            int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
                
            // Player within range
            if(in_range(player.int_x(), player.int_y(), 20)){
                look_at(xyVector, abs_x, abs_y);
                
                if(_idle_time == 0){
                    attack();
                    _idle_time++;
                }else if(_idle_time <= 2*60){
                    _idle_time++;
                }else{
                    _idle_time = 0;
                }
            }else if(in_range(player.int_x(), player.int_y(), 46)){
                look_at(xyVector, abs_x, abs_y);
                if(_idle_time <= 2*60){
                    _idle_time++;
                }
                bn::fixed target_x = this->x();
                bn::fixed target_y = this->y() + 8;
                if((xyVector.x() > 0 && obstacle(x, y, EAST, map)) || (xyVector.x() < 0 && obstacle(x, y, WEST, map))){
                    target_x += xyVector.x()*_stats.speed;
                }
                if((xyVector.y() > 0 && obstacle(x, y, SOUTH, map)) || (xyVector.y() < 0 && obstacle(x, y, NORTH, map))){
                    target_y += xyVector.y()*_stats.speed;
                }

                set_position(target_x, target_y, 8);
            }

            // Random direction
            else{
                if(_idle_time == 0){
                    _dir = randomizer.get_int(12);
                    _idle_time++;
                }else if(_idle_time <= 1*60 + _dir*2){
                    _idle_time++;
                }else{
                    _idle_time = 0;
                }

                // If direction is valid
                if(_dir != Direction::NEUTRAL && _dir < 9){
                    // Move if dir not obstructed
                    if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH, map)){          // UP
                        bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                        set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
                    }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH, map)){  // DOWN
                        bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                        set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
                    }
                    if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST, map)){  // LEFT
                        bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                        set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
                    }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST, map)){ // RIGHT
                        bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                        set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
                    }
                }
                
            }

            
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!_attack_cooldown){
            _attack_cooldown = 20;
            set_animation(frames::Attack, bn::sprite_items::bad_cat.tiles_item());
            _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                                 this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
        if(attack_ended()){
            set_animation(frames::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
    }
    
    static constexpr basic_stats _stats = {1, 1, 3, bn::fixed(0.4)};

};

class PaleTongue: public Enemy{
public:
    ~PaleTongue(){ reset_graphics();}
    // Constructor
    PaleTongue(bn::point position, bn::camera_ptr cam):
        Enemy(position)
        {
            hp = _stats.max_hp;
            if(on_screen(cam)){
                bn::sprite_builder builder(bn::sprite_items::pale_tongue);
                builder.set_position(position.x(), position.y() - 8);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::pale_tongue.tiles_item(), frames::WalkAttack_do[frames::Walk]);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update(jv::Player& player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul) override;
    
    void got_hit(int damage){
        _state = State::HURTING;
        _prev_dir = 0;
        _dir = 0;
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::pale_tongue.tiles_item(), frames::hurt);
        }
    }
    
private:
    void animation_update(){
        if(_prev_dir != _dir){
            set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
        }
        _prev_dir = _dir;
    }
    
    void move(game_map& map, bn::random& randomizer){
        // Decide direction at random
        if(!_attack_cooldown){
            // Random direction
            if(_idle_time == 0){
                _dir = randomizer.get_int(16);
                _idle_time++;
            }else if(_idle_time <= 2*60 + _dir*2){
                _idle_time++;
            }else{
                _idle_time = 0;
            }

            int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

            // If direction is valid
            if(_dir != Direction::NEUTRAL && _dir < 9){
                // Move if dir not obstructed
                if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH, map)){          // UP
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                    set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
                }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH, map)){  // DOWN
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                    set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
                }
                if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST, map)){  // LEFT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                    set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
                }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST, map)){ // RIGHT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                    set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
                }
            }
            
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!_attack_cooldown){
            _idle_time = 0;
            _attack_cooldown = 35;
            set_animation(frames::Attack, bn::sprite_items::pale_tongue.tiles_item(), 8);
            _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                                 this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
        if(attack_ended()){
            set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
        }
    }
    
    static constexpr basic_stats _stats = {2, 1, 5, bn::fixed(0.3)};

};

class PaleFinger: public Enemy{
public:
    ~PaleFinger(){ reset_graphics();}
    // Constructor
    PaleFinger(bn::point position, bn::camera_ptr cam):
        Enemy(position)
        {
            hp = _stats.max_hp;
            if(on_screen(cam, 32, 32)){
                bn::sprite_builder builder(bn::sprite_items::pale_finger);
                builder.set_position(position.x(), position.y() - 26);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::pale_finger.tiles_item(), frames::WalkAttack_do[frames::Walk]);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update(jv::Player& player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul) override;
    
    void got_hit(int damage){
        _state = State::HURTING;
        _prev_dir = 0;
        _dir = 0;
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::pale_finger.tiles_item(), frames::hurt);
        }
    }
    
private:
    void animation_update(){
        if(_prev_dir != _dir){
            set_animation(frames::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
        }
        _prev_dir = _dir;
    }
    
    void move(game_map& map, bn::random& randomizer){
        // Decide direction at random
        if(!_attack_cooldown){
            // Random direction
            if(_idle_time == 0){
                _dir = randomizer.get_int(16);
                _idle_time++;
            }else if(_idle_time <= 2*60 + _dir*2){
                _idle_time++;
            }else{
                _idle_time = 0;
            }

            int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
                
            // If direction is valid
            if(_dir != Direction::NEUTRAL && _dir < 9){
                // Move if dir not obstructed
                if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH, map)){           // UP
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                    set_position(this->x(), this->y() + 26 - _stats.speed*diagonal, 26); 
                }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH, map)){     // DOWN
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                    set_position(this->x(), this->y() + 26 + _stats.speed*diagonal, 26);
                }
                if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST, map)){             // LEFT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                    set_position(this->x() - _stats.speed*diagonal, this->y() + 26, 26);
                }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST, map)){       // RIGHT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                    set_position(this->x() + _stats.speed*diagonal, this->y() + 26, 26);
                }
            }
            
            if(_state == State::NORMAL){ animation_update();}
        }
    }
    
    void attack(){
        if(!_attack_cooldown){
            _idle_time = 0;
            _attack_cooldown = 35;
            set_animation(frames::Attack, bn::sprite_items::pale_finger.tiles_item(), 8);
            _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                                 this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
        }
    }
    
    void attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
        if(attack_ended()){
            set_animation(frames::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
        }
    }
    
    static constexpr basic_stats _stats = {2, 1, 5, bn::fixed(0.3)};

};

class NPC: public Actor{
public:
    ~NPC(){}
    // Constructor
    NPC(bn::point position, bn::camera_ptr cam):
        Actor(bn::rect(position.x(), position.y() + 8, 20, 20))
        {
            bool isOnScreen = on_screen(cam);
            if(isOnScreen){
                bn::sprite_builder builder(bn::sprite_items::cow);
                builder.set_position(position.x(), position.y() - 8);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                constexpr uint16_t arr[4] = {0, 1, 2, 3};
                _animation = bn::sprite_animate_action<MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::cow.tiles_item(), arr);
            }
        }
    
    // Getters
    [[nodiscard]] inline bool on_screen(bn::camera_ptr cam) const {
        const uint8_t halfWidth = 16, halfHeight = 16;
        constexpr uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
        bool up = this->int_y() > cam.y() - y_offset, down = this->int_y() < cam.y() + y_offset;
        bool left = this->int_x() > cam.x() - x_offset, right = this->int_x() < cam.x() + x_offset;
        return left && right && up && down;
    }

    // Setters

    // Functionality
    void update(jv::Player& player, bn::camera_ptr cam, jv::stairs& stairs, bool objective);
    
private:

};

}

bool jv::stairs::climb(bn::rect playerRect, bool playerState){
    bool isOnStairs = playerRect.intersects(_rect) && bn::keypad::a_pressed();
    if(isOpen && playerState == Actor::State::NORMAL && isOnStairs){
        return true;
    }else{
        return false;
    }
}

#endif