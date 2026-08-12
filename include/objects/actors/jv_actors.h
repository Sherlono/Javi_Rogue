#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_rect.h"
#include "bn_size.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_sound_items.h"
#include "bn_sprite_builder.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "jv_math.h"
#include "jv_constants.h"
#include "jv_actors_data.h"


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
    ~Actor();
    // Constructor
    Actor(const uint8_t actor_id, const bn::point p);

    enum Direction { NEUTRAL, NORTH, SOUTH, WEST, NORTHWEST, SOUTHWEST, EAST, NORTHEAST, SOUTHEAST};
    enum State { NORMAL, ATTACKING, HURTING, CHARGING, DEAD};
    
    // Getters
    [[nodiscard]] int x() const { return _rect.x();}
    [[nodiscard]] int y() const { return _rect.y();}
    [[nodiscard]] bn::point position() const { return _rect.position();}
    [[nodiscard]] bn::rect& rect() { return _rect;}
    [[nodiscard]] bn::sprite_ptr& sprite();
    [[nodiscard]] int get_graphics_key() {return graphics_key;}
    [[nodiscard]] bool is_in_range(const int point_x, const int point_y, const int r) const {
        int delta_x = point_x - x(), delta_y = point_y - y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool is_in_range(const bn::point position, const int r) const {
        int delta_x = position.x() - x(), delta_y = position.y() - y();
        return  delta_x*delta_x + delta_y*delta_y <= r*r;
    }
    [[nodiscard]] bool is_on_screen() const;

    // Setters
    void set_position(const bn::fixed x, const bn::fixed y);
    void set_position(const bn::fixed_point point);
    void set_visible(bool visible);
    void set_camera(bn::camera_ptr& c);

    //Functionality
    void look_at(bn::fixed_point normalizedVector){
        bn::fixed abs_x = bn::abs(normalizedVector.x()), abs_y = bn::abs(normalizedVector.y());
        //_prev_dir = _dir;
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
    void load_graphics(const animation::Id action);
    void rotate_rect(){
        bool was_ho = _prev_dir == Direction::EAST || _prev_dir == Direction::WEST;
        bool is_ho = _dir == Direction::EAST || _dir == Direction::WEST;
        if(is_ho != was_ho){
            int rect_width = _rect.width();
            _rect.set_width(_rect.height());
            _rect.set_height(rect_width);
        }
    }

    struct basic_stats{
        constexpr basic_stats(const uint8_t att, const uint8_t def, const uint8_t maxhp, const bn::fixed spe):
            attack(att), defense(def), max_hp(maxhp), speed(spe){}
        uint8_t attack, defense, max_hp;
        bn::fixed speed;
    };

    struct Graphics: public bn::sprite_ptr{
        // Constructor
        Graphics();
        
        // Setters
        static void configure_animation(const uint8_t actor_id, const uint8_t dir, const animation::Id act, const bn::fixed_point pos); // Meant to set up the construction of a new instance
        bn::sprite_animate_action<animation::MAX_FRAMES> create_animation();
        
        static bool horizontal_flip;
        static animation::Id action;
        static uint16_t sprite_item_id;
        static bn::fixed_point sprite_position;
        static animation_type frames;

        bn::sprite_animate_action<animation::MAX_FRAMES> animation;
    };

    uint8_t id, graphics_key;
protected:
    [[nodiscard]] bool _map_obstacle(const uint8_t direction);
    
    uint8_t _prev_dir = SOUTH, _dir = SOUTH;
    bn::rect _rect;
};

namespace Actor_data{
    constexpr bn::array<Actor::basic_stats, ACTOR_COUNT - NPCS_COUNT> stats = {
        Actor::basic_stats(1, 1, 5, bn::fixed(1.5)),    // good cat
        Actor::basic_stats{1, 1, 3, bn::fixed(1.1)},    // bad cat
        Actor::basic_stats{2, 1, 5, bn::fixed(0.3)},    // pale tongue
        Actor::basic_stats{2, 1, 5, bn::fixed(0.3)},    // pale finger
        Actor::basic_stats{1, 1, 3, bn::fixed(1.1)}     // snakes
    };
}


class Enemy: public Actor{
public:
    ~Enemy();
    // Constructor
    Enemy(const uint8_t actor_id, bn::point pos_and_dimensions):
        Actor(actor_id, pos_and_dimensions),
        hp(Actor_data::stats[id].max_hp) {}

    // Getters
    [[nodiscard]] bool alive() const { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking(int duration) const { return _attack_cooldown > duration;}
    [[nodiscard]] inline bool attack_ended(int duration) {
        return _attack_cooldown == duration;
    }
    [[nodiscard]] uint8_t get_state() const { return _state;}
    [[nodiscard]] uint8_t get_hp() const { return hp;}

    // Setters
    void set_state(int s){ _state = s;}

    // Functionality
    void got_hit(int attack);
    void update();

private:
    void _start_attack(Graphics& my_graphics);
    void _attack_update(Graphics& my_graphics);

    void _movement(Graphics& my_graphics);
    void _displace(const bn::fixed speed, bn::fixed_point position);
    
    void _simple_fsm_update(Graphics& my_graphics);

protected:
    uint8_t _state = State::NORMAL, _idle_time = 0, hp;
    int8_t _prev_attack_cooldown = 0, _attack_cooldown = 0;
};

class NPC: public Actor{
public:
    virtual ~NPC();
    // Constructor
    NPC(const uint8_t actor_id, const bn::point p): Actor(actor_id, p) {};

    // Functionality
    virtual void update() {};
private:

};

}

#endif