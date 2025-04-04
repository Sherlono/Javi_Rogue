#include "jv_actors.h"

namespace jv{
// ************ Player ************
// Constructor
Player::Player(int x, int y, bn::random* random_ref, game_map* m_r, bn::camera_ptr cam):
    Actor(bn::sprite_items::character.create_sprite(0 , 0 - 8), x, y, bn::rect(x, y, 10, 10)),
    _state(State::NORMAL),
    _stats(basic_stats(5, 5, 1, 1, bn::fixed(1.5))),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 
                                                        frames::w_do[0], frames::w_do[1], frames::w_do[2], frames::w_do[3])),
    _hitbox(bn::rect(x, y, 10, 10)),
    _prev_attack_cooldown(0),
    _attack_cooldown(0),
    _prev_dir(2),
    _dir(2),
    _map_ref(m_r),
    _randomizer(random_ref)
{
    _sprite.set_bg_priority(1);
    cam.set_position(x, y);
}
// Setters
void Player::set_position(bn::fixed x, bn::fixed y, bool sprite_follow){
    if(sprite_follow){ _sprite.set_position(x, y - 8);}
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void Player::set_position(bn::point point, bool sprite_follow){
    if(sprite_follow){ _sprite.set_position(point.x(), point.y() - 8);}
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

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
// Constructor
Enemy::Enemy(int x, int y, bn::random* random_ref, game_map* m_r, bn::camera_ptr cam):
    Actor(bn::sprite_items::enemy.create_sprite(x, y - 8), x, y, bn::rect(x, y, 10, 10), cam),
    _state(State::NORMAL),
    _stats(basic_stats(5, 5, 1, 1, bn::fixed(0.4))),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(),
                                                        frames::w_do[0], frames::w_do[1], frames::w_do[2], frames::w_do[3])),
    _hitbox(bn::rect(x, y, 10, 10)),
    _prev_dir(2),
    _dir(2),
    _idle_time(0),
    _map_ref(m_r),
    _randomizer(random_ref)
{
    _sprite.set_bg_priority(1);
}
// Setters
void Enemy::set_position(bn::fixed x, bn::fixed y){
    _sprite.set_position(x, y - 8);
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void Enemy::set_position(bn::point point){
    _sprite.set_position(point.x(), point.y() - 8);
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

void Enemy::update(jv::Player* player){
    // Change sprite priority to draw behind or above player
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
            jv::Dialog::init("I am the evil cat. I will attack", "you in a future version of", "the game.");
        }
        // Combat
        if(player->is_attacking() && player->get_hitbox().intersects(rect())){
            got_hit(player->get_attack());
            player->_state = State::NORMAL;
        }
}
}

// ************** NPC **************
// Constructor
NPC::NPC(int x, int y, bn::camera_ptr cam):
    Actor(bn::sprite_items::cow.create_sprite(x, y - 8), x, y, bn::rect(x, y + 8, 20, 20), cam)
{
    _sprite.set_bg_priority(1);
}
// Setters
void NPC::set_position(bn::fixed x, bn::fixed y){
    _sprite.set_position(x, y - 8);
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void NPC::set_position(bn::point point){
    _sprite.set_position(point.x(), point.y() - 8);
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

void NPC::update(jv::Player* player){
    priority_update(player->y());
    
    if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
        jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
    }
}
}