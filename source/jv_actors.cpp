#include "jv_actors.h"

namespace jv{
// ************ Player ************
void Player::update(bn::camera_ptr cam, bool noClip){
    end_attack();
    move(cam, noClip);

    if(!_attack_cooldown && _prev_attack_cooldown != _attack_cooldown){
        insert_animation(frames::w_up, frames::w_ho, frames::w_do);
    }

    if(_animation.done()){ animation_update();}
    // Combat
    if(bn::keypad::b_pressed()){
        attack();
    }
}

// ************* Enemy *************
void Enemy::update(jv::Player* player){
    if(_state != State::DEAD){
        priority_update(player->y());

        if(_state != State::HURTING){
            move();
        }else{
            if(!_animation.done()){ _animation.update();
            }else{
                _state = State::NORMAL;
                insert_animation(frames::idle, frames::idle, frames::idle);
            }
        }

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
            player->set_state(State::NORMAL);
        }
}
}

// ************** NPC **************
void NPC::update(jv::Player* player){
    priority_update(player->y());
    
    // Dialog
    if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
        jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
    }
    _animation.update();
}
}