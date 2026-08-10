#ifndef JV_PLAYER_H
#define JV_PLAYER_H

#include "jv_actors.h"
#include "jv_inventory.h"

namespace jv{
class Player: public Actor{
public:
    ~Player() = default;
    // Constructor
    Player(bn::point position);

    // Getters
    [[nodiscard]] bool alive() { return _state != State::DEAD;}
    [[nodiscard]] bool is_attacking() { return _attack_cooldown > 0;}
    [[nodiscard]] inline bool attack_ended() {
        return !is_attacking() && _prev_attack_cooldown != _attack_cooldown;
    }
    [[nodiscard]] bool can_interact() const { return _interact_token;}
    [[nodiscard]] bool moved() const { return _moved;}
    [[nodiscard]] bn::point prev_position() const { return _prev_pos;}
    [[nodiscard]] bn::fixed_point normalized_vector(bn::point entity_pos){
        return jv::normalize(position() - entity_pos);
    }

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
    void set_enemies_ptr(bn::ivector<Enemy>* enemies) { _enemies_ref = enemies;}

    // Functionality
    void reset();
    void got_hit(int damage, bool ignoreDef = false);
    void update();

    void heal(int h){
        _hp = bn::min(_hp + h, int(_stats.max_hp));
    }
    void spend_interact_token(){
        _interact_token = false;
    }

    bool invulnerable = false, noClip = false;
    Inventory playerInventory;

private:
    bool _enemy_obstacle(const int x, const int y, const uint8_t direction);
    void _movement();
    void _start_attack();
    void _attack_update();

    bool _interact_token = true, _moved = false;
    uint8_t _state = State::NORMAL;
    uint8_t _prev_dir = SOUTH;
    int8_t _prev_attack_cooldown = 0, _attack_cooldown = 0;
    uint8_t _hp;
    basic_stats _stats;
    bn::point _prev_pos;
    bn::rect _hitbox;
    bn::ivector<Enemy>* _enemies_ref;
};

}

#endif