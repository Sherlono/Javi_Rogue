#include "jv_player.h"

#include "jv_global.h"

#include "bn_sprite_items_good_cat.h"

namespace jv{
Player::Player(bn::point position): // Constructor
    Actor(Actor_data::Id::Player, position),
    _stats(basic_stats(1, 1, 5, bn::fixed(1.5))),
    _hitbox(bn::rect(position.x(), position.y(), 10, 10))
    {
        _hp = _stats.max_hp;
        
        /*bn::sprite_builder builder(bn::sprite_items::good_cat);
        builder.set_position(position.x(), position.y() - Actor_data::meta[Actor_data::Id::Player].sprt_y_offset);
        //builder.set_camera(Global::Camera()); // *** Do not set camera from Global before it is initialized ***
        builder.set_bg_priority(1);
        
        Global::Graphics().push_back(Actor::Graphics(builder.release_build(), id, animation::Walk_do[Actor_data::meta[id].anim_set]));
        Global::Actor_Graphic(0).set_animation(id, _dir, jv::animation::Id::Walk);

        Global::Graphics()[0].sprite = builder.release_build();
        Global::Graphics()[0].animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 4, bn::sprite_items::good_cat.tiles_item(), animation::idle[0]);*/

        //load_graphics(jv::animation::Id::Walk);

        has_graphics = true;
    }

void Player::reset(){
    _prev_dir = NEUTRAL;
    _dir = SOUTH;
    _moved = false;
    _hitbox.set_position(x(), y());
    Actor::Graphics& player_graphics = Global::Graphics_Manager()[graphics_key];
    player_graphics.configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
    player_graphics.animation.update();
}

bool Player::_enemy_obstacle(const int x, const int  y, const uint8_t direction){
    switch(direction){
        case NORTH:{
            bn::point p(x, y - 6);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i].rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case SOUTH:{
            bn::point p(x, y + 6);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i].rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case WEST:{
            bn::point p(x - 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i].rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case EAST:{
            bn::point p(x + 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i].rect().contains(p)){
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
        Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Attack, position());
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
        Global::Graphics_Manager()[graphics_key].configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
    }
}

void Player::_movement(){
    bool up_held    = bn::keypad::up_held(),
         down_held  = bn::keypad::down_held(),
         left_held  = bn::keypad::left_held(),
         right_held = bn::keypad::right_held();

    if(up_held || down_held || left_held || right_held){
        _dir = 1*up_held + 2*down_held + 3*left_held + 6*right_held;

        // Move if dir not obstructed
        if(up_held && (noClip || (_map_obstacle(NORTH) && _enemy_obstacle(x(), y(), NORTH)))){         // Up
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = Global::Graphics_Manager()[graphics_key].y() - _stats.speed*diagonal;
            set_position(Global::Graphics_Manager()[graphics_key].x(), target_y);
        }else if(down_held && (noClip || (_map_obstacle(SOUTH) && _enemy_obstacle(x(), y(), SOUTH)))){ // Down
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = Global::Graphics_Manager()[graphics_key].y() + _stats.speed*diagonal;
            set_position(Global::Graphics_Manager()[graphics_key].x(), target_y);
        }
        if(left_held && (noClip || (_map_obstacle(WEST) && _enemy_obstacle(x(), y(), WEST)))){         // Left
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = Global::Graphics_Manager()[graphics_key].x() - _stats.speed*diagonal;
            set_position(target_x, Global::Graphics_Manager()[graphics_key].y());
        }else if(right_held && (noClip || (_map_obstacle(EAST) && _enemy_obstacle(x(), y(), EAST)))){  // Right
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = Global::Graphics_Manager()[graphics_key].x() + _stats.speed*diagonal;
            set_position(target_x, Global::Graphics_Manager()[graphics_key].y());
        }
        _hitbox.set_position(x() - 10*(_dir == NORTHWEST || _dir == SOUTHWEST) + 10*(_dir == NORTHEAST || _dir == SOUTHEAST) - 16*(_dir == WEST) + 16*(_dir == EAST),
                            y() - 10*(_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) + 10*(_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST));
    }
    
    if(_state == State::NORMAL){
        if(_prev_dir != _dir){
            Global::Graphics_Manager()[graphics_key].configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
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
                Global::Graphics_Manager()[graphics_key].configure_animation(Actor_data::Id::Player, _dir, animation::Id::Walk, position());
            }
        }else if(!is_attacking()) [[likely]] {
            _movement();
        }
        
        _moved = _prev_pos != position();

        // Combat
        if(bn::keypad::b_pressed()) [[unlikely]] {
            _start_attack();
        }
        
        if(!Global::Graphics_Manager()[graphics_key].animation.done()) [[likely]] { Global::Graphics_Manager()[graphics_key].animation.update();}
        _prev_dir = _dir;
    }
}

}