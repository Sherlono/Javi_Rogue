#include "jv_actors.h"

namespace jv{
// ************ Player ************
void Player::update(bn::camera_ptr& cam, game_map& map, bool noClip){
    if(alive()){
        attack_update();
        if(get_state() == State::HURTING){
            if(_animation->done()){
                set_state(State::NORMAL);
                set_animation(frames::Walk, bn::sprite_items::character.tiles_item());
            }
        }else{
            move(cam, map, noClip);
            if(attack_ended()){
                set_animation(frames::Walk, bn::sprite_items::character.tiles_item());
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

// ************* BadCat *************
void BadCat::update(jv::Player* player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul = false){
    if(on_screen(cam)){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::enemy);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(cam);
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
            if(alive()){
                set_animation(frames::Walk, bn::sprite_items::enemy.tiles_item());
            }else{
                _sprite->set_tiles(bn::sprite_items::enemy.tiles_item().create_tiles(24));
            }
        }

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
                        set_animation(0, bn::sprite_items::enemy.tiles_item());
                    }
                    else{ set_animation(frames::Walk, bn::sprite_items::enemy.tiles_item());}
                }
            }else{
                move(map, randomizer);
                if(_idle_time == 30 && _dir < 9 && _dir != 0){ attack();}
                if(attack_ended()){
                    set_animation(frames::Walk, bn::sprite_items::enemy.tiles_item());
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
    }else{
        if(_sprite.has_value()){
            _sprite.reset();
            _animation.reset();
        }
    }
}

// ************* PaleTongue *************
void PaleTongue::update(jv::Player* player, bn::camera_ptr& cam, game_map& map, bn::random& randomizer, bool isInvul = false){
    if(on_screen(cam)){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::pale_tongue);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(cam);
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
            if(alive()){
                set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
            }else{
                _sprite->set_tiles(bn::sprite_items::pale_tongue.tiles_item().create_tiles(24));
            }
        }

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
                        set_animation(0, bn::sprite_items::pale_tongue.tiles_item(), 8);
                    }
                    else{ set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);}
                }
            }else{
                move(map, randomizer);
                if(_idle_time == 30 && _dir < 9 && _dir != 0){ attack();}
                if(attack_ended()){
                    set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
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
    }else{
        if(_sprite.has_value()){
            _sprite.reset();
            _animation.reset();
        }
    }
}

// ************** NPC **************
void NPC::update(jv::Player& player, bn::camera_ptr cam, jv::stairs& stairs, bool objective){
    if(on_screen(cam)){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(cam);
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build();
            _animation = bn::create_sprite_animate_action_forever(_sprite.value(), 8,
                                    bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
        }

        if(player.sprite().y() > _sprite->y()){
            _sprite->set_z_order(player.sprite().z_order() + 1);
        }else{
            _sprite->set_z_order(player.sprite().z_order() - 1);
        }

        if(player.get_state() == State::NORMAL && !player.is_attacking()){
            // Dialog
            if(bn::keypad::a_pressed() && player.rect().intersects(rect())){
                if(!objective){
                    jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
                }else{
                    jv::Dialog::init("Thanks for getting rid of the evil", "creatures! The stairs are open now!");
                    if(!stairs.isOpen){
                        stairs.set_open(objective);
                    }
                }
            }
        }

        _animation->update();
    }else{
        if(_sprite.has_value()){
            _sprite.reset();
            _animation.reset();
        }
    }
    
}
}