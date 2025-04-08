#include "jv_actors.h"

namespace jv{
// ************ Player ************
void Player::update(bool noClip){
    
    if(_state != State::DEAD){
        attack_update();
        if(_state == State::HURTING){
            if(_animation.done()){
                _state = State::NORMAL;
                insert_animation(frames::w_up, frames::w_ho, frames::w_do);
            }
        }else{
            move(noClip);
            if(!_attack_cooldown && _prev_attack_cooldown != _attack_cooldown){
                insert_animation(frames::w_up, frames::w_ho, frames::w_do);
            }
            if(_animation.done()){ animation_update();}
        }
        if(!_animation.done()){ _animation.update();}

        // Combat
        if(bn::keypad::b_pressed()){
            attack();
        }
    }
}

// ************* Enemy *************
void Enemy::update(jv::Player* player){
    y_priority(player->y() + 4*(_state == State::DEAD));

    if(_state != State::DEAD){
        attack_update();

        // Movement and Animations
        if(_state == State::HURTING){
            if(_animation.done()){
                _state = State::NORMAL;
                if(!_idle_time){ insert_animation(frames::idle, frames::idle, frames::idle);}
                else{insert_animation(frames::w_up, frames::w_ho, frames::w_do);}
            }
        }else{
            move();
            if((_idle_time == 30 && _dir < 9) || bn::keypad::l_pressed()){ attack();}
            if(!_attack_cooldown && _prev_attack_cooldown != _attack_cooldown){
                insert_animation(frames::w_up, frames::w_ho, frames::w_do);
            }
            if(_animation.done()){ animation_update();}
        }
        if(!_animation.done()){ _animation.update();}

        // Dialog
        if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
            if(_stats.hp == _stats.max_hp){
                jv::Dialog::init("I am the evil cat. I will attack", "you in a future version of", "the game.");
            }else{
                bn::string_view line1 = "Hey watch it! I only have " + bn::to_string<30>(_stats.hp) + " hp";
                jv::Dialog::init(line1, "left!");
            }
        }
        // Combat
        if(player->is_attacking() && player->get_hitbox().intersects(rect())){
            got_hit(player->get_attack());
            player->set_state(player->get_state() == State::HURTING ? State::HURTING : State::NORMAL);
        }
        if(is_attacking() && get_hitbox().intersects(player->rect())){
            player->got_hit(get_attack());
            set_state(get_state() == State::HURTING ? State::HURTING : State::NORMAL);
        }
}
}

// ************** NPC **************
void NPC::update(jv::Player* player){
    y_priority(player->y());
    
    // Dialog
    if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
        jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
    }
    _animation.update();
}
}