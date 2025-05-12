#include "jv_actors.h"

namespace jv{
// ************ Player ************
void Player::update(bool noClip){
    if(alive()){
        attack_update();
        if(get_state() == State::HURTING){
            if(_animation->done()){
                set_state(State::NORMAL);
                insert_animation(frames::Walk);
            }
        }else{
            move(noClip);
            if(attack_ended()){
                insert_animation(frames::Walk);
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
void BadCat::update(jv::Player* player, bn::camera_ptr cam, bool isInvul = false){
    bool isOnScreen = on_screen(cam);
    if(!isOnScreen){
        if(_sprite){
            _sprite.reset();
            _animation.reset();
        }
    }else if(!_sprite){
        bn::sprite_builder builder(bn::sprite_items::enemy);
        builder.set_position(this->int_x(), this->int_y() - 8);
        builder.set_camera(cam);
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build(); 
        if(alive()){
            insert_animation(frames::Walk);
        }else{
            _sprite->set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(24));
        }
    }

    if(isOnScreen){
        if(player->sprite().y() > _sprite->y()){ _sprite->set_z_order(player->sprite().z_order() + 1);}
        else{ _sprite->set_z_order(player->sprite().z_order() - 1);}

        if(alive()){
            attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                        insert_animation(0);
                    }
                    else{insert_animation(frames::Walk);}
                }
            }else{
                move();
                if(_idle_time == 30 && _dir < 9 && _dir != 0){ attack();}
                if(attack_ended()){
                    insert_animation(frames::Walk);
                }
                if(_animation->done()){ animation_update();}
            }
            if(!_animation->done()){ _animation->update();}
            
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
    bool isOnScreen = on_screen(cam);
    if(!isOnScreen){
        if(_sprite){
            _sprite.reset();
            _animation.reset();
        }
    }else if(!_sprite){
        bn::sprite_builder builder(bn::sprite_items::cow);
        builder.set_position(this->int_x(), this->int_y() - 8);
        builder.set_camera(cam);
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build();
        _animation = bn::create_sprite_animate_action_forever(_sprite.value(), 8,
                                bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
    }


    if(isOnScreen){
        if(player.sprite().y() > _sprite->y()){
            _sprite->set_z_order(player.sprite().z_order() + 1);
        }else{
            _sprite->set_z_order(player.sprite().z_order() - 1);
        }
        _animation->update();
    }

    
    if(player.get_state() == State::NORMAL && !player.is_attacking()){
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