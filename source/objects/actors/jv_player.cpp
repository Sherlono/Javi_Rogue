#include "jv_player.h"

#include "bn_log.h"
#include "jv_global.h"

#include "bn_sprite_items_good_cat.h"

namespace jv{
Player::Player(bn::point position): // Constructor
    Actor(Actor_data::Id::Player, position),
    _stats(basic_stats(1, 1, 5, bn::fixed(1.5))),
    _hitbox(bn::rect(position.x(), position.y(), 10, 10))
    {
        _hp = _stats.max_hp;
    }

void Player::reset_at(bn::point p){
    set_position(p);
    _prev_dir = NEUTRAL;
    _dir = SOUTH;
    _moved = false;
    _hitbox.set_position(x(), y() + 16);
    
    /*BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Nonexistent sprite is about to be accessed.");
    Actor::Graphics& player_graphics = Global::Graphics_Manager()[graphics_key];

    Actor::Graphics::configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
    player_graphics.animation = player_graphics.create_animation();
    player_graphics.animation.update();*/
}

bool Player::_enemy_obstacle(const int x, const int  y, const uint8_t direction){
    enemy_ref_t enemies_ref = Global::Enemies();
    switch(direction){
        case NORTH:{
            bn::point p(x, y - 6);
            for(int i = 0; i < enemies_ref.size(); i++){
                if(enemies_ref[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case SOUTH:{
            bn::point p(x, y + 6);
            for(int i = 0; i < enemies_ref.size(); i++){
                if(enemies_ref[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case WEST:{
            bn::point p(x - 8, y);
            for(int i = 0; i < enemies_ref.size(); i++){
                if(enemies_ref[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case EAST:{
            bn::point p(x + 8, y);
            for(int i = 0; i < enemies_ref.size(); i++){
                if(enemies_ref[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        default:
            BN_ERROR("Invalid direction: ", direction);
            break;
    }
    return true;
}

void Player::got_hit(int damage, bool ignoreDef){
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
            Global::Graphics_Manager()[graphics_key].animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, bn::sprite_items::good_cat.tiles_item(), animation::hurt);
        }else{
            bn::sound_items::death.play(0.5);
            _state = State::DEAD;
            sprite().set_horizontal_flip(false);
            sprite().set_tiles(bn::sprite_items::good_cat.tiles_item().create_tiles(23));
            _hitbox.set_position(_rect.position());
        }
    }
}

void Player::_start_attack(){
    if(!is_attacking() && _state != State::HURTING){
        bn::sound_items::swipe.play(0.5);
        _attack_cooldown = 20;
        Actor::Graphics::configure_animation(id, _dir, animation::Id::Attack, position());
        Global::Graphics_Manager()[graphics_key].animation = Global::Graphics_Manager()[graphics_key].create_animation();
    }
}

void Player::_attack_update(){
    _prev_attack_cooldown = _attack_cooldown;
    if(_attack_cooldown){ _attack_cooldown--;}
    if(_state != State::HURTING){
        _state = State::NORMAL;
        if(_attack_cooldown == 1){ _state = State::ATTACKING;}
    }
    if(attack_ended()){
        Actor::Graphics::configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
        Global::Graphics_Manager()[graphics_key].animation = Global::Graphics_Manager()[graphics_key].create_animation();
    }
}

void Player::_movement(){
    Actor::Graphics& player_graphics = Global::Graphics_Manager()[graphics_key];
    bool up_held    = bn::keypad::up_held(),
         down_held  = bn::keypad::down_held(),
         left_held  = bn::keypad::left_held(),
         right_held = bn::keypad::right_held();

    if(up_held || down_held || left_held || right_held){
        _dir = 1*up_held + 2*down_held + 3*left_held + 6*right_held;

        // Move if dir not obstructed
        if(up_held && (noClip || (_map_obstacle(NORTH) && _enemy_obstacle(x(), y(), NORTH)))){         // Up
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = player_graphics.y() - _stats.speed*diagonal;
            set_position(player_graphics.x(), target_y);
        }else if(down_held && (noClip || (_map_obstacle(SOUTH) && _enemy_obstacle(x(), y(), SOUTH)))){ // Down
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = player_graphics.y() + _stats.speed*diagonal;
            set_position(player_graphics.x(), target_y);
        }
        if(left_held && (noClip || (_map_obstacle(WEST) && _enemy_obstacle(x(), y(), WEST)))){         // Left
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = player_graphics.x() - _stats.speed*diagonal;
            set_position(target_x, player_graphics.y());
        }else if(right_held && (noClip || (_map_obstacle(EAST) && _enemy_obstacle(x(), y(), EAST)))){  // Right
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = player_graphics.x() + _stats.speed*diagonal;
            set_position(target_x, player_graphics.y());
        }
        _hitbox.set_position(x() - 10*(_dir == NORTHWEST || _dir == SOUTHWEST) + 10*(_dir == NORTHEAST || _dir == SOUTHEAST) - 16*(_dir == WEST) + 16*(_dir == EAST),
                            y() - 10*(_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) + 10*(_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST));
    }
    
    if(_state == State::NORMAL){
        if(_prev_dir != _dir){
            Actor::Graphics::configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
            player_graphics.animation = player_graphics.create_animation();
        }
    }
}

void Player::update(){
    if(alive()) /*Heh*/ [[likely]] {
        _interact_token = true;
        _prev_pos = position();
        
        _attack_update();
        if(get_state() == State::HURTING) [[unlikely]] {
            if(Global::Graphics_Manager()[graphics_key].animation.done()) [[unlikely]] {
                set_state(State::NORMAL);
                Actor::Graphics &player_graphics = Global::Graphics_Manager()[graphics_key];
                Actor::Graphics::configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
                player_graphics.animation = player_graphics.create_animation();
            }
        }else if(!is_attacking()) [[likely]] {
            _movement();
        }
        
        _moved = _prev_pos != position();

        // Combat
        if(bn::keypad::b_pressed()) [[unlikely]] {
            _start_attack();
        }
        
        _prev_dir = _dir;
    }
}

}