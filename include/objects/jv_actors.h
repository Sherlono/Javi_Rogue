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

#include "bn_sprite_items_good_cat.h"
#include "bn_sprite_items_bad_cat.h"
#include "bn_sprite_items_pale_tongue.h"
#include "bn_sprite_items_pale_finger.h"
#include "bn_sprite_items_cow.h"
#include "bn_sprite_items_fox.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

class GameMap;

namespace jv{
class tiled_bg;

using animation_type = bn::span<const uint16_t>;

class Actor{
public:
    virtual ~Actor() = default;
    // Constructor
    Actor(bn::rect r): _rect(r){}

    enum Direction { NEUTRAL, NORTH, SOUTH, WEST, NORTHWEST, SOUTHWEST, EAST, NORTHEAST, SOUTHEAST};
    enum State { NORMAL, ATTACKING, HURTING, CHARGING, DEAD};
    
    struct Graphics{
        // Getters
        [[nodiscard]] bn::fixed x() const { return sprite->x();}
        [[nodiscard]] bn::fixed y() const { return sprite->y();}
        [[nodiscard]] bn::fixed_point position() const { return sprite->position();}
        [[nodiscard]] int z_order() const { return sprite->z_order();}

        void set_animation(const uint8_t dir, const uint8_t option, const bn::sprite_tiles_item& tiles, const uint8_t wait_frames = 4){
            animation_type frames;
            if(dir == NORTH || dir == NORTHWEST || dir == NORTHEAST){        // UP
                sprite->set_horizontal_flip(false);
                frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_up) : animation_type(animation::Attack_up);
            }else if(dir == SOUTH || dir == SOUTHWEST || dir == SOUTHEAST){  // DOWN
                sprite->set_horizontal_flip(false);
                frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_do) : animation_type(animation::Attack_do);
            }else if(dir == WEST){                            // LEFT
                sprite->set_horizontal_flip(true);
                frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_ho) : animation_type(animation::Attack_ho);
            }else if(dir == EAST){                            // RIGHT
                sprite->set_horizontal_flip(false);
                frames = (option == animation::Id::Walk) ? animation_type(animation::Walk_ho) : animation_type(animation::Attack_ho);
            }else if(option == animation::Id::Idle){
                sprite->set_horizontal_flip(false);
                frames = animation_type(animation::idle);
            }else{
                sprite->set_horizontal_flip(false);
                frames = animation_type(animation::Walk_do);
            }
            
            if(option != animation::Attack){
                animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite.value(), wait_frames, tiles, frames);
            }else{
                animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite.value(), wait_frames, tiles, frames);
            }
        }

        void reset(){
            sprite.reset();
            animation.reset();
        }

        bn::optional<bn::sprite_ptr> sprite;
        bn::optional<bn::sprite_animate_action<animation::MAX_FRAMES>> animation;
    };

    // Getters
    [[nodiscard]] int x() const { return _rect.x();}
    [[nodiscard]] int y() const { return _rect.y();}
    [[nodiscard]] bn::point position() const { return _rect.position();}
    [[nodiscard]] bn::rect& rect() { return _rect;}
    [[nodiscard]] bn::sprite_ptr& sprite() { return graphics.sprite.value();}
    [[nodiscard]] bool is_in_range(const int point_x, const int point_y, const int r) const {
        int delta_x = point_x - x(), delta_y = point_y - y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool is_in_range(const bn::point position, const int r) const {
        int delta_x = position.x() - x(), delta_y = position.y() - y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool is_on_screen(uint8_t halfWidth = 16, uint8_t halfHeight = 16) const;
    // Setters
    void set_position(const bn::fixed x, const bn::fixed y){
        sprite().set_position(x, y);
        _rect.set_position(x.floor_integer(), y.floor_integer() + _sprite_y_offset());
    }
    void set_position(const bn::fixed_point point){
        sprite().set_position(point.x(), point.y());
        _rect.set_position(point.x().floor_integer(), point.y().floor_integer() + _sprite_y_offset());
    }
    void set_visible(bool visible){
        if(graphics.sprite.has_value()){ sprite().set_visible(visible);};
    }
    void set_camera(bn::camera_ptr& c){
        graphics.sprite->set_camera(c);
    }

    void look_at(bn::fixed_point normalizedVector){
        bn::fixed abs_x = bn::abs(normalizedVector.x()), abs_y = bn::abs(normalizedVector.y());
        _prev_dir = _dir;
        if(normalizedVector.y() < -0.5){
            if(abs_y > abs_x){
                _dir = NORTH;
            }else if(normalizedVector.x() > 0){
                _dir = NORTHEAST;
            }else{
                _dir = NORTHWEST;
            }
        }else if(normalizedVector.y() > 0.5){
            if(abs_y > abs_x){
                _dir = SOUTH;
            }else if(normalizedVector.x() > 0){
                _dir = SOUTHEAST;
            }else{
                _dir = SOUTHWEST;
            }
        }else{
            if(normalizedVector.x() > 0){
                _dir = EAST;
            }else{
                _dir = WEST;
            }
        }
    }
    
    void load_graphics(const uint8_t option);

    Graphics graphics;
protected:
    struct basic_stats{
        constexpr basic_stats(const uint8_t att, const uint8_t def, const uint8_t maxhp, const bn::fixed spe):
            attack(att), defense(def), max_hp(maxhp), speed(spe){}
        uint8_t attack, defense;
        uint8_t max_hp;
        bn::fixed speed;
    };
    
    [[nodiscard]] bool _map_obstacle(const uint8_t direction);
    [[nodiscard]] virtual int _sprite_y_offset() const = 0;
    [[nodiscard]] virtual int _wait_frames() const = 0;
    [[nodiscard]] virtual const bn::sprite_item& _sprite_item() = 0;
    
    uint8_t _prev_dir = SOUTH, _dir = SOUTH;
    bn::rect _rect;
};

class Player: public Actor{
public:
    ~Player(){ graphics.reset();};
    // Constructor
    Player(bn::point position);

    // Getters
    [[nodiscard]] bool alive() { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking() { return _attack_cooldown > 0;}
    [[nodiscard]] inline bool attack_ended() { return !is_attacking() && _prev_attack_cooldown != _attack_cooldown;}
    [[nodiscard]] bool can_interact() const { return _interact_token;}
    [[nodiscard]] bool moved() const { return _moved;}
    [[nodiscard]] bn::point prev_position() const { return _prev_pos;}

    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] uint8_t get_attack() const { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() const { return _stats.defense;}
    [[nodiscard]] bn::fixed  get_speed() const { return _stats.speed;}
    [[nodiscard]] uint8_t  get_maxhp() const { return _stats.max_hp;}
    [[nodiscard]] uint8_t  get_hp() const { return _hp;}
    [[nodiscard]] bn::rect& get_hitbox() { return _hitbox;}

    [[nodiscard]] uint8_t* get_hp_ptr() { return &_hp;}
    [[nodiscard]] uint8_t* get_maxhp_ptr() { return &_stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}
    void set_enemies_ptr(bn::ivector<Enemy*>* enemies) { _enemies_ref = enemies;}

    // Functionality
    void reset(){
        _prev_dir = NEUTRAL;
        _dir = SOUTH;
        _moved = false;
        _hitbox.set_position(x(), y());
        graphics.set_animation(_dir, animation::Walk, bn::sprite_items::good_cat.tiles_item());
        graphics.animation->update();
    }
    
    void update();

    void heal(int h){
        _hp = bn::min(_hp + h, int(_stats.max_hp));
    }

    void got_hit(int damage, bool ignoreDef = false){
        if(!invulnerable){
            _state = State::HURTING;
            _attack_cooldown = 0;
            _prev_attack_cooldown = 0;

            uint8_t dmg;
            if(ignoreDef) dmg = damage;
            else dmg = damage/_stats.defense;
            _hp = dmg > _hp ? 0 : _hp - dmg;

            if(_hp > 0) [[likely]] {
                sprite().set_horizontal_flip(_dir == WEST);
                graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, bn::sprite_items::good_cat.tiles_item(), animation::hurt);
            }else{
                bn::sound_items::death.play(0.5);
                _state = State::DEAD;
                sprite().set_horizontal_flip(false);
                sprite().set_tiles(bn::sprite_items::good_cat.tiles_item().create_tiles(23));
                _hitbox.set_position(_rect.position());
            }
        }
    }
    
    void spend_interact_token(){
        _interact_token = false;
    }

    bool invulnerable = false, noClip = false;
    Inventory playerInventory;

private:
    bool _enemy_obstacle(const int x, const int y, const uint8_t direction);
    
    void _movement();
    
    void _start_attack(){
        if(!is_attacking() && _state != State::HURTING){
            bn::sound_items::swipe.play(0.5);
            _attack_cooldown = 20;
            graphics.set_animation(_dir, animation::Attack, bn::sprite_items::good_cat.tiles_item());
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
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::good_cat.tiles_item());
        }
    }
    
    [[nodiscard]] int _sprite_y_offset() const override { return 8;}
    [[nodiscard]] int _wait_frames() const override { return 4;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::good_cat;}

    bool _interact_token = true, _moved = false;
    uint8_t _state = State::NORMAL;
    uint8_t _prev_dir = SOUTH;
    int8_t _prev_attack_cooldown = 0, _attack_cooldown = 0;
    uint8_t _hp;
    basic_stats _stats;
    bn::point _prev_pos;
    bn::rect _hitbox;
    bn::ivector<Enemy*>* _enemies_ref;
};

class Enemy: public Actor{
public:
    ~Enemy(){ graphics.reset();}
    Enemy(uint8_t max_hp, bn::point position): Actor(bn::rect(position.x(), position.y(), 16, 16)), hp(max_hp) {}

    // Getters
    [[nodiscard]] bool alive() const { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking(int duration) const { return _attack_cooldown > duration;}
    [[nodiscard]] inline bool attack_ended(int duration) {
        return _attack_cooldown == duration;
    }
    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] uint8_t get_hp() const { return hp;}

    // Functionality
    virtual void update() = 0;

    void displace(const bn::fixed speed){   
        // If direction is valid
        if(_dir != NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) && _map_obstacle( NORTH)){          // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == NORTHEAST);
                set_position(graphics.x(), graphics.y() - speed*diagonal); 
            }else if((_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST) && _map_obstacle(SOUTH)){  // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == SOUTHWEST || _dir == SOUTHEAST);
                set_position(graphics.x(), graphics.y() + speed*diagonal);
            }
            if((_dir == WEST || _dir == NORTHWEST || _dir == SOUTHWEST) && _map_obstacle(WEST)){  // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == SOUTHWEST);
                set_position(graphics.x() - speed*diagonal, graphics.y());
            }else if((_dir == EAST || _dir == NORTHEAST || _dir == SOUTHEAST) && _map_obstacle(EAST)){ // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHEAST || _dir == SOUTHEAST);
                set_position(graphics.x() + speed*diagonal, graphics.y());
            }
        }
    }
    
protected:
    uint8_t hp;
    uint8_t _state = State::NORMAL;
    int8_t _prev_attack_cooldown = 0, _attack_cooldown = 0;
    uint8_t _idle_time = 0;
};

class BadCat: public Enemy{
public:
    ~BadCat(){ graphics.reset();}
    // Constructor
    BadCat(bn::point position);

    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] uint8_t get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        const uint8_t dmg = damage/_stats.defense;
        hp = dmg > hp ? 0 : hp - dmg;
        if(hp > 0) [[likely]] {
            _state = State::HURTING;
            sprite().set_horizontal_flip(_dir == WEST);
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, bn::sprite_items::bad_cat.tiles_item(), animation::hurt);
        }else{
            _state = State::DEAD;
        }
    }
    
private:
    void _movement();
    
    void _start_attack(){
        if(!_attack_cooldown){
            _attack_cooldown = 60;
            graphics.set_animation(_dir, animation::Attack, bn::sprite_items::bad_cat.tiles_item());
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
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
    }
    
    [[nodiscard]] int _sprite_y_offset() const override { return 8;}
    [[nodiscard]] int _wait_frames() const override { return 4;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::bad_cat;}

    static constexpr basic_stats _stats = {1, 1, 3, bn::fixed(1.3)};

};

class PaleTongue: public Enemy{
public:
    ~PaleTongue(){ graphics.reset();}
    // Constructor
    PaleTongue(bn::point position);
    
    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] uint8_t get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        const uint8_t dmg = damage/_stats.defense;
        hp = dmg > hp ? 0 : hp - dmg;
        if(hp > 0) [[likely]] {
            _state = State::HURTING;
            sprite().set_horizontal_flip(_dir == WEST);
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, bn::sprite_items::pale_tongue.tiles_item(), animation::hurt);
        }else{
            _state = State::DEAD;
        }
    }
    
private:
    void _movement();
    
    void _start_attack(){
        if(!_attack_cooldown){
            _attack_cooldown = 75;
            graphics.set_animation(_dir, animation::Attack, bn::sprite_items::pale_tongue.tiles_item(), 8);
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
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
        }
    }
    
    [[nodiscard]] int _sprite_y_offset() const override { return 8;}
    [[nodiscard]] int _wait_frames() const override { return 8;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::pale_tongue;}

    static constexpr basic_stats _stats = {2, 1, 5, bn::fixed(0.3)};
};

class PaleFinger: public Enemy{
public:
    ~PaleFinger(){ graphics.reset();}
    // Constructor
    PaleFinger(bn::point position);

    // Getters
    [[nodiscard]] uint8_t get_attack() { return _stats.attack;}
    [[nodiscard]] uint8_t get_defense() { return _stats.defense;}
    [[nodiscard]] uint8_t get_maxhp() { return _stats.max_hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void update() override;
    
    void got_hit(int damage){
        _attack_cooldown = 0;
        _prev_attack_cooldown = 0;
        const uint8_t dmg = damage/_stats.defense;
        hp = dmg > hp ? 0 : hp - dmg;
        if(hp > 0) [[likely]] {
            _state = State::HURTING;
            sprite().set_horizontal_flip(_dir == WEST);
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, bn::sprite_items::pale_finger.tiles_item(), animation::hurt);
        }else{
            _state = State::DEAD;
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
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
        }
    }
    
    [[nodiscard]] int _sprite_y_offset() const override { return 24;}
    [[nodiscard]] int _wait_frames() const override { return 8;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::pale_finger;}

    static constexpr basic_stats _stats = {2, 1, 5, bn::fixed(0.3)};
};

class NPC: public Actor{
public:
    ~NPC(){}
    // Constructor
    NPC(bn::rect r): Actor(r) {}

    // Functionality
    virtual void update();
    
private:

};

class Cow: public NPC{
public:
    ~Cow(){}
    // Constructor
    Cow(bn::point position);

    // Functionality
    void update() override;
    
private:
    [[nodiscard]] int _sprite_y_offset() const override { return 8;}
    [[nodiscard]] int _wait_frames() const override { return 8;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::cow;}
};

class Fox: public NPC{
public:
    ~Fox(){}
    // Constructor
    Fox(bn::point position);
    
    // Functionality
    void force_move_player();
    void present_choice();
    void update() override;
    
private:
    [[nodiscard]] int _sprite_y_offset() const override { return 8;}
    [[nodiscard]] int _wait_frames() const override { return 8;}
    [[nodiscard]] const bn::sprite_item& _sprite_item() override { return bn::sprite_items::fox;}
};

}

#endif