#include "jv_actors.h"

namespace jv{
// ************ Player ************
void Player::update(bool noClip){
    
    if(alive()){
        attack_update();
        if(_state == State::HURTING){
            if(_animation->done()){
                _state = State::NORMAL;
                insert_animation(frames::w_up, frames::w_ho, frames::w_do);
            }
        }else{
            move(noClip);
            if(!_attack_cooldown && _prev_attack_cooldown != _attack_cooldown){
                insert_animation(frames::w_up, frames::w_ho, frames::w_do);
            }
            if(_animation->done()){ animation_update();}
        }
        if(!_animation->done()){ _animation->update();}

        // Combat
        if(bn::keypad::b_pressed()){
            attack();
        }
    }
}

// ************* Enemy *************
void Enemy::update(jv::Player* player, bn::camera_ptr cam, bool isInvul){
    int halfWidth = 16, halfHeight = 16;
    bool up = this->int_y() > cam.y() - 80 - halfHeight, down = this->int_y() < cam.y() + 80 + halfHeight;
    bool left = this->int_x() > cam.x() - 120 - halfWidth, right = this->int_x() < cam.x() + 120 + halfWidth;
    bool onScreen = left && right && up && down;
    if(!onScreen){
        if(_sprite){
            _sprite.reset();
            _animation.reset();
        }
    }else if(!_sprite){
        bn::sprite_builder builder(bn::sprite_items::enemy);
        builder.set_position(this->int_x(), this->int_y() - 8);
        builder.set_camera(cam);
        builder.set_bg_priority(1);
        builder.set_blending_enabled(true);
        _sprite = builder.release_build(); 
        if(alive()){
            insert_animation(frames::w_up, frames::w_ho, frames::w_do);
        }else{
            _sprite->set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(24));
        }
    }

    if(onScreen){
        if(player->_sprite->y() > _sprite->y()){ _sprite->set_z_order(player->_sprite->z_order() + 1);}
        else{ _sprite->set_z_order(player->_sprite->z_order() - 1);}

        if(alive()){
            attack_update();

            // Movement and Animations
            if(_state == State::HURTING){
                if(_animation->done()){
                    _state = State::NORMAL;
                    if(!_idle_time){ insert_animation(frames::idle, frames::idle, frames::idle);}
                    else{insert_animation(frames::w_up, frames::w_ho, frames::w_do);}
                }
            }else{
                move();
                if(_idle_time == 30 && _dir < 9 && _dir != 0){ attack();}
                if(!_attack_cooldown && _prev_attack_cooldown != _attack_cooldown){
                    insert_animation(frames::w_up, frames::w_ho, frames::w_do);
                }
                if(_animation->done()){ animation_update();}
            }
            if(!_animation->done()){ _animation->update();}

            if(player->get_state() == State::NORMAL){
                // Dialog
                if(bn::keypad::a_pressed() && player->rect().intersects(rect())){
                    if(get_hp() == get_maxhp()){
                        jv::Dialog::init("I am the evil cat. My best friend", "is Neko Arc. Don't mess with me!");
                    }else{
                        bn::string_view line1 = "Hey watch it! I only have " + bn::to_string<30>(get_hp()) + " hp";
                        jv::Dialog::init(line1, "left!");
                    }
                }
            }
            // Combat
            if(player->alive()){
                if(player->get_state() == State::ATTACKING && player->get_hitbox().intersects(rect())){
                    got_hit(player->get_attack());
                    player->set_state(player->get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
                if(!isInvul && get_state() == State::ATTACKING && get_hitbox().intersects(player->rect())){
                    player->got_hit(get_attack());
                    set_state(get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
            }
        }
    }
}

// ************** NPC **************
void NPC::update(jv::Player& player, bn::camera_ptr cam, jv::stairs& stairs, bool objective){
    int halfWidth = 16, halfHeight = 16;
    bool up = this->int_y() > cam.y() - 80 - halfHeight, down = this->int_y() < cam.y() + 80 + halfHeight;
    bool left = this->int_x() > cam.x() - 120 - halfWidth, right = this->int_x() < cam.x() + 120 + halfWidth;
    bool onScreen = left && right && up && down;
    if(!onScreen){
        if(_sprite){
            _sprite.reset();
            _animation.reset();
        }
    }else if(!_sprite){
        bn::sprite_builder builder(bn::sprite_items::cow);
        builder.set_position(this->int_x(), this->int_y() - 8);
        builder.set_camera(cam);
        builder.set_bg_priority(1);
        builder.set_blending_enabled(true);
        _sprite = builder.release_build();
        _animation = bn::create_sprite_animate_action_forever(_sprite.value(), 8,
                                bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
    }


    if(onScreen){
        if(player._sprite->y() > _sprite->y()){
            _sprite->set_z_order(player._sprite->z_order() + 1);
        }else{
            _sprite->set_z_order(player._sprite->z_order() - 1);
        }
        _animation->update();
    }

    
    if(player.get_state() == State::NORMAL){
        // Dialog
        if(bn::keypad::a_pressed() && player.rect().intersects(rect())){
            if(!objective){
                jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
            }else{
                jv::Dialog::init("Thanks for getting rid of the evil", "cats! The stairs are open now!");
                if(!stairs.isOpen){
                    stairs.set_open(objective);
                }
            }
        }
    }
}
}