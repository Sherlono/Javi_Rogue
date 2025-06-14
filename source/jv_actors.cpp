#include "jv_actors.h"

namespace jv{
// ************ Actor ************
[[nodiscard]] bool Actor::obstacle(int x, int y, const uint8_t direction){
    switch(direction){
        case Direction::NORTH:
            return jv::Common::Map().cell(x, y - 1) > 0 && jv::Common::Map().cell(x, y - 1) < WTILES_COUNT;
            break;
        case Direction::SOUTH:
            return jv::Common::Map().cell(x, y + 1) > 0 && jv::Common::Map().cell(x, y + 1) < WTILES_COUNT;
            break;
        case Direction::WEST:
            return jv::Common::Map().cell(x - 1, y) > 0 && jv::Common::Map().cell(x - 1, y) < WTILES_COUNT;
            break;
        case Direction::EAST:
            return jv::Common::Map().cell(x + 1, y) > 0 && jv::Common::Map().cell(x + 1, y) < WTILES_COUNT;
            break;
        default:
            BN_ASSERT(false, "Invalid direction", direction);
            return false;
            break;
    }
}

// ************ Player ************
void Player::move(bool noClip){
    if(!_attack_cooldown){
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();

            int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

            // Move if dir not obstructed
            if(bn::keypad::up_held() && (noClip || obstacle(x, y, NORTH))){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = this->y() - _stats.speed*diagonal;
                set_position(this->x(), target_y + 8, 8);
            }else if(bn::keypad::down_held() && (noClip || obstacle(x, y, SOUTH))){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = this->y() + _stats.speed*diagonal;
                set_position(this->x(), target_y + 8, 8);
            }
            if(bn::keypad::left_held() && (noClip || obstacle(x, y, WEST))){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = this->x() - _stats.speed*diagonal;
                set_position(target_x, this->y() + 8, 8);
            }else if(bn::keypad::right_held() && (noClip || obstacle(x, y, EAST))){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = this->x() + _stats.speed*diagonal;
                set_position(target_x, this->y() + 8, 8);
            }
            _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                                this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
        }

        if(_state == State::NORMAL){ animation_update();}
    }
}

void Player::update(bool noClip){
    if(alive()){
        _interact_token = true;
        
        attack_update();
        if(get_state() == State::HURTING){
            if(_animation->done()){
                set_state(State::NORMAL);
                set_animation(frames::Walk, bn::sprite_items::good_cat.tiles_item());
            }
        }else{
            move(noClip);
            if(_animation->done()){ animation_update();}
        }
        if(!_animation->done()){ _animation->update();}

        // Combat
        if(bn::keypad::b_pressed()){
            attack();
        }
    }

    // Camera
    bn::fixed t = bn::fixed(0.13);
    jv::Common::Camera().set_position(jv::lerp(jv::Common::Camera().x(), _hitbox.x(), t), jv::lerp(jv::Common::Camera().y(), _hitbox.y() + 8, t));
}

// ************* BadCat *************
void BadCat::move(){
    // Decide direction at random
    if(!_attack_cooldown){
        bn::fixed_point xyVector = jv::normalize(jv::Common::Player().position() - position());
        bn::fixed abs_x = bn::abs(xyVector.x()), abs_y = bn::abs(xyVector.y());
        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
            
        // Player within range
        if(in_range(jv::Common::Player().int_x(), jv::Common::Player().int_y(), 20)){
            look_at(xyVector, abs_x, abs_y);
            
            if(_idle_time == 0){
                attack();
                _idle_time++;
            }else if(_idle_time <= 2*60){
                _idle_time++;
            }else{
                _idle_time = 0;
            }
        }else if(in_range(jv::Common::Player().int_x(), jv::Common::Player().int_y(), 46)){
            look_at(xyVector, abs_x, abs_y);
            if(_idle_time <= 2*60){
                _idle_time++;
            }
            bn::fixed target_x = this->x();
            bn::fixed target_y = this->y() + 8;
            if((xyVector.x() > 0 && obstacle(x, y, EAST)) || (xyVector.x() < 0 && obstacle(x, y, WEST))){
                target_x += xyVector.x()*_stats.speed;
            }
            if((xyVector.y() > 0 && obstacle(x, y, SOUTH)) || (xyVector.y() < 0 && obstacle(x, y, NORTH))){
                target_y += xyVector.y()*_stats.speed;
            }

            set_position(target_x, target_y, 8);
        }

        // Random direction
        else{
            if(_idle_time == 0){
                _dir = jv::Common::Random().get_int(12);
                _idle_time++;
            }else if(_idle_time <= 1*60 + _dir*2){
                _idle_time++;
            }else{
                _idle_time = 0;
            }

            // If direction is valid
            if(_dir != Direction::NEUTRAL && _dir < 9){
                // Move if dir not obstructed
                if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH)){          // UP
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                    set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
                }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH)){  // DOWN
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                    set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
                }
                if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST)){  // LEFT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                    set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
                }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST)){ // RIGHT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                    set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
                }
            }
            
        }
        
        if(_state == State::NORMAL){ animation_update();}
    }
}

void BadCat::update(bool isInvul = false){
    if(on_screen(jv::Common::Camera())){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::bad_cat);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(jv::Common::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
            if(alive()){
                set_animation(frames::Walk, bn::sprite_items::bad_cat.tiles_item());
            }else{
                _sprite->set_tiles(bn::sprite_items::bad_cat.tiles_item().create_tiles(24));
            }
        }

        if(jv::Common::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(jv::Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(jv::Common::Player().sprite().z_order() - 1);}

        if(alive()){
            attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                        set_animation(0, bn::sprite_items::bad_cat.tiles_item());
                    }
                    else{ set_animation(frames::Walk, bn::sprite_items::bad_cat.tiles_item());}
                }
            }else{
                move();
                if(_animation->done()){ animation_update();}
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(jv::Common::Player().alive()){
                if(jv::Common::Player().get_state() == State::ATTACKING && jv::Common::Player().get_hitbox().intersects(rect())){
                    got_hit(jv::Common::Player().get_attack());
                    jv::Common::Player().set_state(jv::Common::Player().get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
                if(!isInvul && get_state() == State::ATTACKING && get_hitbox().intersects(jv::Common::Player().rect())){
                    jv::Common::Player().got_hit(get_attack());
                    set_state(get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
            }
        }
    }else{
        if(_sprite.has_value()){
            reset_graphics();
        }
    }
}

// ************* PaleTongue *************
void PaleTongue::move(){
    // Decide direction at random
    if(!_attack_cooldown){
        // Random direction
        if(_idle_time == 0){
            _dir = jv::Common::Random().get_int(16);
            _idle_time++;
        }else if(_idle_time <= 2*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

        // If direction is valid
        if(_dir != Direction::NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH)){          // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
            }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH)){  // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
            }
            if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST)){  // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
            }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST)){ // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
            }
        }
        
        if(_state == State::NORMAL){ animation_update();}
    }
}
    
void PaleTongue::update(bool isInvul = false){
    if(on_screen(jv::Common::Camera())){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::pale_tongue);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(jv::Common::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
            if(alive()){
                set_animation(frames::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
            }else{
                _sprite->set_tiles(bn::sprite_items::pale_tongue.tiles_item().create_tiles(24));
            }
        }

        if(jv::Common::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(jv::Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(jv::Common::Player().sprite().z_order() - 1);}

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
                move();
                if(_idle_time == 30 && _dir < 9 && _dir != 0){ attack();}
                if(_animation->done()){ animation_update();}
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(jv::Common::Player().alive()){
                if(jv::Common::Player().get_state() == State::ATTACKING && jv::Common::Player().get_hitbox().intersects(rect())){
                    got_hit(jv::Common::Player().get_attack());
                    jv::Common::Player().set_state(jv::Common::Player().get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
                if(!isInvul && get_state() == State::ATTACKING && get_hitbox().intersects(jv::Common::Player().rect())){
                    jv::Common::Player().got_hit(get_attack());
                    set_state(get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
            }
        }
    }else{
        if(_sprite.has_value()){
            reset_graphics();
        }
    }
}

// ************* PaleFinger *************
void PaleFinger::move(){
    // Decide direction at random
    if(!_attack_cooldown){
        // Random direction
        if(_idle_time == 0){
            _dir = jv::Common::Random().get_int(16);
            _idle_time++;
        }else if(_idle_time <= 2*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
            
        // If direction is valid
        if(_dir != Direction::NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && obstacle(x, y, NORTH)){           // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                set_position(this->x(), this->y() + 26 - _stats.speed*diagonal, 26); 
            }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && obstacle(x, y, SOUTH)){     // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                set_position(this->x(), this->y() + 26 + _stats.speed*diagonal, 26);
            }
            if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && obstacle(x, y, WEST)){             // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                set_position(this->x() - _stats.speed*diagonal, this->y() + 26, 26);
            }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && obstacle(x, y, EAST)){       // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                set_position(this->x() + _stats.speed*diagonal, this->y() + 26, 26);
            }
        }
        
        if(_state == State::NORMAL){ animation_update();}
    }
}

void PaleFinger::attack(){
    if(!(_attack_cooldown + _idle_time)){
        _attack_cooldown = 35;
        set_animation(frames::Attack, bn::sprite_items::pale_finger.tiles_item(), 8);
        jv::Common::create_projectile(this->int_x(), this->int_y() - 40);
    }
}

void PaleFinger::update(bool isInvul = false){
    if(on_screen(jv::Common::Camera(), 32, 32)){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::pale_finger);
            builder.set_position(this->int_x(), this->int_y() - 26);
            builder.set_camera(jv::Common::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
            if(alive()){
                set_animation(frames::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
            }else{
                _sprite->set_tiles(bn::sprite_items::pale_finger.tiles_item().create_tiles(24));
            }
        }

        if(jv::Common::Player().sprite().y() > _sprite->y() + 8){ _sprite->set_z_order(jv::Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(jv::Common::Player().sprite().z_order() - 1);}

        if(alive()){
            attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                        set_animation(0, bn::sprite_items::pale_finger.tiles_item(), 8);
                    }
                    else{ set_animation(frames::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);}
                }
            }else{
                move();
                int player_x = jv::Common::Player().int_x(), player_y = jv::Common::Player().int_y();

                if(jv::Common::Player().alive() && in_range(player_x, player_y, 60)){ attack();}

                if(_animation->done()){ animation_update();}
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(jv::Common::Player().alive()){
                if(jv::Common::Player().get_state() == State::ATTACKING && jv::Common::Player().get_hitbox().intersects(rect())){
                    got_hit(jv::Common::Player().get_attack());
                    jv::Common::Player().set_state(jv::Common::Player().get_state() == State::HURTING ? State::HURTING : State::NORMAL);
                }
            }
        }
    }else{
        if(_sprite.has_value()){
            reset_graphics();
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
            if(bn::keypad::a_pressed() && player.rect().intersects(rect()) && player._interact_token){
                if(!objective){
                    jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
                }else{
                    jv::Dialog::init("Thanks for getting rid of the evil", "creatures! The stairs are open now!");
                    if(!stairs.isOpen){
                        stairs.set_open(true);
                    }
                }
                player._interact_token = false;
            }
        }

        _animation->update();
    }else{
        if(_sprite.has_value()){
            reset_graphics();
        }
    }
    
}
}