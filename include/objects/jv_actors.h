#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_rect.h"
#include "bn_array.h"
#include "bn_assert.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_sound_items.h"
#include "bn_sprite_builder.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "jv_inventory.h"

#include "bn_sprite_items_cow.h"
#include "bn_sprite_items_bad_cat.h"
#include "bn_sprite_items_good_cat.h"
#include "bn_sprite_items_pale_tongue.h"
#include "bn_sprite_items_pale_finger.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

class GameMap;

namespace jv{
class Enemy;
struct stairs;
class tiled_bg;

using animation_type = bn::span<const uint16_t>;

class Actor{
public:
    virtual ~Actor() = default;
    // Constructor
    Actor(bn::rect r): _prev_dir(Direction::SOUTH), _dir(Direction::SOUTH), _rect(r){}

    enum Direction { NEUTRAL, NORTH, SOUTH, WEST, NORTHWEST, SOUTHWEST, EAST, NORTHEAST, SOUTHEAST};
    enum State { NORMAL, ATTACKING, HURTING, CHARGING, DEAD};
    
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
    [[nodiscard]] bn::point int_position() const{ return _rect.position();}
    [[nodiscard]] bn::fixed_point position() const{ return _sprite->position();}
    [[nodiscard]] bn::rect& rect() { return _rect;}
    [[nodiscard]] bn::sprite_ptr& sprite() { return _sprite.value();}
    [[nodiscard]] bn::sprite_animate_action<animation::MAX_FRAMES>& animation() { return _animation.value();}
    [[nodiscard]] bool in_range(int x, int y, const int r) const {
        int delta_x = x - int_x(), delta_y = y - int_y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool in_range(bn::point position, const int r) const {
        int delta_x = position.x() - int_x(), delta_y = position.y() - int_y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool map_obstacle(int x, int y, const uint8_t direction);

    // Setters
    void set_position(bn::fixed x, bn::fixed y, uint16_t y_offset = 8){
        _sprite->set_position(x, y - y_offset);
        _rect.set_position(x.integer(), y.integer());
    }
    void set_position(bn::fixed_point point, uint16_t y_offset = 8){
        _sprite->set_position(point.x(), point.y() - y_offset);
        _rect.set_position(point.x().integer(), point.y().integer());
    }
    void set_visible(bool visible){
        if(_sprite.has_value()){ _sprite->set_visible(visible);};
    }

    // For actions with direction
    void set_animation(const uint8_t option, const bn::sprite_tiles_item& tiles, const uint8_t wait_frames = 4){
        animation_type frames;
        if(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST){        // UP
            _sprite->set_horizontal_flip(false);
            frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_up) : animation_type(animation::Attack_up);
        }else if(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST){  // DOWN
            _sprite->set_horizontal_flip(false);
            frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_do) : animation_type(animation::Attack_do);
        }else if(_dir == Direction::WEST){                            // LEFT
            _sprite->set_horizontal_flip(true);
            frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_ho) : animation_type(animation::Attack_ho);
        }else if(_dir == Direction::EAST){                            // RIGHT
            _sprite->set_horizontal_flip(false);
            frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_ho) : animation_type(animation::Attack_ho);
        }else{
            _sprite->set_horizontal_flip(false);
            frames = animation_type(animation::idle);
        }

        if(option != animation::Attack){
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), wait_frames, tiles, frames);
        }else{
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(_sprite.value(), wait_frames, tiles, frames);
        }
    }

    void look_at(bn::fixed_point& xyVector){
        bn::fixed abs_x = bn::abs(xyVector.x()), abs_y = bn::abs(xyVector.y());
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

    void reset_graphics(){
        _sprite.reset();
        _animation.reset();
    }

    void load_graphics(const bn::sprite_item& item, int y_offset, int wait_frames);

protected:
    uint8_t _prev_dir, _dir;
    bn::rect _rect;
    bn::optional<bn::sprite_ptr> _sprite;
    bn::optional<bn::sprite_animate_action<animation::MAX_FRAMES>> _animation;
};

class Player: public Actor{
public:
    ~Player(){ reset_graphics();};
    // Constructor
    Player(bn::point position, bn::camera_ptr& camera, bn::ivector<Enemy*>* enemies):
        Actor(bn::rect(position.x(), position.y(), 16, 16)),
        invulnerable(false),
        playerInventory(),
        _interact_token(true),
        _state(State::NORMAL),
        _prev_dir(Direction::SOUTH),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _stats(basic_stats(1, 1, 5, bn::fixed(1.5))),
        _hitbox(bn::rect(position.x(), position.y(), 10, 10)),
        _enemies_ref(enemies)
        {
            _hp = _stats.max_hp;
            bn::sprite_builder builder(bn::sprite_items::good_cat);
            builder.set_position(position.x(), position.y() - 8);
            builder.set_camera(camera);
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build();
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), 4, bn::sprite_items::good_cat.tiles_item(), animation::idle);
        }
    
    // Getters
    [[nodiscard]] bool alive() { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking() { return bool(_attack_cooldown);}
    [[nodiscard]] inline bool attack_ended() { return !is_attacking() && _prev_attack_cooldown != _attack_cooldown;}
    [[nodiscard]] bool can_interact() const { return _interact_token;}

    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] int get_attack() const { return _stats.attack;}
    [[nodiscard]] int get_defense() const { return _stats.defense;}
    [[nodiscard]] int get_maxhp() const { return _stats.max_hp;}
    [[nodiscard]] int get_hp() const { return _hp;}
    [[nodiscard]] bn::rect& get_hitbox() { return _hitbox;}

    [[nodiscard]] short* get_hp_ptr() { return &_hp;}
    [[nodiscard]] short* get_maxhp_ptr() { return &_stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void reset(){
        _prev_dir = Direction::NEUTRAL;
        _dir = Direction::SOUTH;
        _hitbox.set_position(this->int_x(), this->int_y());
        set_animation(animation::Walk, bn::sprite_items::good_cat.tiles_item());
        _animation->update();
    }

    void update(bool noClip = false);

    void heal(int h){
        _hp = bn::min(_hp + h, int(_stats.max_hp));
    }

    void got_hit(int damage, bool ignoreDef = false){
        if(!invulnerable){
            _state = State::HURTING;
            _attack_cooldown = 0;
            _prev_attack_cooldown = 0;

            if(ignoreDef){ _hp -= damage;}
            else{ _hp -= damage/_stats.defense;}

            if(_hp <= 0){
                bn::sound_items::death.play(0.5);
                _state = State::DEAD;
                _sprite->set_horizontal_flip(false);
                _sprite->set_tiles(bn::sprite_items::good_cat.tiles_item().create_tiles(23));
                _hitbox.set_position(_rect.position());
            }else{
                //bn::sound_items::death.play(0.5);
                _sprite->set_horizontal_flip(_dir == Direction::WEST);
                _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::good_cat.tiles_item(), animation::hurt);
            }
        }
    }
    
    void set_interact_token(const bool t){
        _interact_token = t;
    }

    bool invulnerable;
    Inventory playerInventory;

private:
    bool _enemy_obstacle(int x, int y, const uint8_t direction);
    
    void _movement(bool noClip = false);
    
    void _start_attack(){
        if(!is_attacking() && _state != State::HURTING){
            bn::sound_items::swipe.play(0.5);
            _attack_cooldown = 20;
            set_animation(animation::Attack, bn::sprite_items::good_cat.tiles_item());
        }
    }
    
    void _attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 1){ _state = State::ATTACKING;}
        }
        if(attack_ended()){
            set_animation(animation::Walk, bn::sprite_items::good_cat.tiles_item());
        }
    }
    
    bool _interact_token;
    uint8_t _state;
    uint8_t _prev_dir;
    int8_t _prev_attack_cooldown, _attack_cooldown;
    short _hp;
    basic_stats _stats;
    bn::rect _hitbox;
    bn::ivector<Enemy*>* _enemies_ref;
};

class Enemy: public Actor{
public:
    ~Enemy(){ reset_graphics();}
    Enemy(bn::point position):
        Actor(bn::rect(position.x(), position.y(), 16, 16)),
        _state(State::NORMAL),
        _prev_attack_cooldown(0),
        _attack_cooldown(0),
        _idle_time(0) {}

    // Getters
    [[nodiscard]] bool alive() const { return _state != State::DEAD;}
    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] short get_hp() const { return hp;}
    [[nodiscard]] bool on_screen(bn::camera_ptr& cam, uint8_t halfWidth = 16, uint8_t halfHeight = 16) const {
        uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
        bool up = this->int_y() > cam.y() - y_offset, down = this->int_y() < cam.y() + y_offset;
        bool left = this->int_x() > cam.x() - x_offset, right = this->int_x() < cam.x() + x_offset;
        return left && right && up && down;
    }
    [[nodiscard]] bool is_attacking(int duration) const { return _attack_cooldown > duration;}
    [[nodiscard]] inline bool attack_ended(int duration) {
        return _attack_cooldown == duration;
    }

    // Functionality
    virtual void update() = 0;

    void displace(int x, int y, const int sprite_offset, const bn::fixed speed){
        // If direction is valid
        if(_dir != Direction::NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && map_obstacle(x, y, NORTH)){          // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                set_position(this->x(), this->y() + sprite_offset - speed*diagonal, sprite_offset); 
            }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, SOUTH)){  // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                set_position(this->x(), this->y() + sprite_offset + speed*diagonal, sprite_offset);
            }
            if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && map_obstacle(x, y, WEST)){  // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                set_position(this->x() - speed*diagonal, this->y() + sprite_offset, sprite_offset);
            }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, EAST)){ // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                set_position(this->x() + speed*diagonal, this->y() + sprite_offset, sprite_offset);
            }
        }
    }
    
protected:
    short hp;
    uint8_t _state;
    int8_t _prev_attack_cooldown, _attack_cooldown;
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
            if(on_screen(cam)){
                bn::sprite_builder builder(bn::sprite_items::bad_cat);
                builder.set_position(position.x(), position.y() - 8);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), 4, bn::sprite_items::bad_cat.tiles_item(), animation::idle);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _state = State::HURTING;
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::bad_cat.tiles_item(), animation::hurt);
        }
    }
    
private:
    void _movement();
    
    void _start_attack(){
        if(!_attack_cooldown){
            _attack_cooldown = 60;
            set_animation(animation::Attack, bn::sprite_items::bad_cat.tiles_item());
        }
    }
    
    void _attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 40){ _state = State::ATTACKING;}
        }
        if(attack_ended(40)){
            set_animation(animation::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
    }
    
    static constexpr uint8_t SPRTYOFFSET = 8;
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
                builder.set_position(position.x(), position.y() - SPRTYOFFSET);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::pale_tongue.tiles_item(), animation::idle);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _state = State::HURTING;
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::pale_tongue.tiles_item(), animation::hurt);
        }
    }
    
private:
    void _movement();
    
    void _start_attack(){
        if(!_attack_cooldown){
            _attack_cooldown = 75;
            set_animation(animation::Attack, bn::sprite_items::pale_tongue.tiles_item(), 8);
        }
    }
    
    void _attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 40){ _state = State::ATTACKING;}
        }
        if(attack_ended(40)){
            set_animation(animation::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
        }
    }
    
    static constexpr uint8_t SPRTYOFFSET = 8;
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
            if(on_screen(cam, 32, 58)){
                bn::sprite_builder builder(bn::sprite_items::pale_finger);
                builder.set_position(position.x(), position.y() - SPRTYOFFSET);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                
                _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::pale_finger.tiles_item(), animation::idle);
            }
        }
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] short get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        hp -= damage/_stats.defense;
        if(hp <= 0){
            _state = State::DEAD;
        }else{
            _state = State::HURTING;
            _sprite->set_horizontal_flip(_dir == Direction::WEST);
            _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(_sprite.value(), 8, bn::sprite_items::pale_finger.tiles_item(), animation::hurt);
        }
    }
    
private:
    void _movement();
    
    void _start_attack();

    void _attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown){ _attack_cooldown--;}
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == 40){ _state = State::ATTACKING;}
        }
        if(attack_ended(40)){
            set_animation(animation::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
        }
    }
    
    static constexpr uint8_t SPRTYOFFSET = 24;
    static constexpr basic_stats _stats = {2, 1, 5, bn::fixed(0.3)};
};

class NPC: public Actor{
public:
    ~NPC(){}
    // Constructor
    NPC(bn::point position, bn::camera_ptr cam):
        Actor(bn::rect(position.x(), position.y() + 8, 20, 20))
        {
            if(on_screen(cam)){
                bn::sprite_builder builder(bn::sprite_items::cow);
                builder.set_position(position.x(), position.y() - 8);
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build();
                constexpr uint16_t arr[4] = {0, 1, 2, 3};
                _animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(_sprite.value(), 8, bn::sprite_items::cow.tiles_item(), arr);
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
    void update(jv::stairs& stairs, tiled_bg& bg, bool objective);
    
private:

};

}

#endif