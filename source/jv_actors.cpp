#include "jv_actors.h"

namespace jv{
// ************ Actor ************
[[nodiscard]] bool Actor::map_obstacle(int x, int y, const uint8_t direction){
    switch(direction){
        case Direction::NORTH:
            return Common::Map().cell(x, y - 1) > 0 && Common::Map().cell(x, y - 1) < WTILES_COUNT;
            break;
        case Direction::SOUTH:
            return Common::Map().cell(x, y + 1) > 0 && Common::Map().cell(x, y + 1) < WTILES_COUNT;
            break;
        case Direction::WEST:
            return Common::Map().cell(x - 1, y) > 0 && Common::Map().cell(x - 1, y) < WTILES_COUNT;
            break;
        case Direction::EAST:
            return Common::Map().cell(x + 1, y) > 0 && Common::Map().cell(x + 1, y) < WTILES_COUNT;
            break;
        default:
            BN_ASSERT(false, "Invalid direction", direction);
            return false;
            break;
    }
}

void Actor::load_graphics(const bn::sprite_item& item, int y_offset, int wait_frames){
    bn::sprite_builder builder(item);
    builder.set_position(this->int_x(), this->int_y() - y_offset);
    builder.set_camera(Common::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
    set_animation(frames::Walk, item.tiles_item(), wait_frames);
}

// ************ Player ************
void Player::move(bool noClip){
    if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
        _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();

        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

        // Move if dir not obstructed
        if(bn::keypad::up_held() && (noClip || map_obstacle(x, y, NORTH))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = this->y() - _stats.speed*diagonal;
            set_position(this->x(), target_y + 8, 8);
        }else if(bn::keypad::down_held() && (noClip || map_obstacle(x, y, SOUTH))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = this->y() + _stats.speed*diagonal;
            set_position(this->x(), target_y + 8, 8);
        }
        if(bn::keypad::left_held() && (noClip || map_obstacle(x, y, WEST))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = this->x() - _stats.speed*diagonal;
            set_position(target_x, this->y() + 8, 8);
        }else if(bn::keypad::right_held() && (noClip || map_obstacle(x, y, EAST))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = this->x() + _stats.speed*diagonal;
            set_position(target_x, this->y() + 8, 8);
        }
        _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                            this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
    }

    if(_state == State::NORMAL){ walking_update(bn::sprite_items::good_cat.tiles_item());}
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
        }else if(!is_attacking()){
            move(noClip);
        }
        if(!_animation->done()){ _animation->update();}

        // Combat
        if(bn::keypad::b_pressed()){
            attack();
        }
    }

    // Camera
    bn::fixed t = bn::fixed(0.13);
    Common::Camera().set_position(jv::lerp(Common::Camera().x(), _hitbox.x(), t), jv::lerp(Common::Camera().y(), _hitbox.y() + 8, t));
}

// ************* BadCat *************
void BadCat::move(){
    // Decide direction at random
    if(!is_attacking(40)){
        bn::fixed_point xyVector = jv::normalize(Common::Player().position() - position());
        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
        
        // Player within range
        if(in_range(Common::Player().int_x(), Common::Player().int_y(), 18)){
            look_at(xyVector);
            attack();
            
            if(_idle_time <= 2*60){
                _idle_time++;
            }else{
                _idle_time = 0;
            }
        }else if(in_range(Common::Player().int_x(), Common::Player().int_y(), 46)){
            look_at(xyVector);
            if(_idle_time <= 2*60){
                _idle_time++;
            }
            bn::fixed target_x = this->x();
            bn::fixed target_y = this->y();
            if((xyVector.x() > 0 && map_obstacle(x, y, EAST)) || (xyVector.x() < 0 && map_obstacle(x, y, WEST))){
                target_x += xyVector.x()*_stats.speed;
            }
            if((xyVector.y() > 0 && map_obstacle(x, y, SOUTH)) || (xyVector.y() < 0 && map_obstacle(x, y, NORTH))){
                target_y += xyVector.y()*_stats.speed;
            }

            set_position(target_x, target_y + 8, 8);
        }

        // Random direction
        else{
            if(_idle_time == 0){
                _dir = Common::Random().get_int(12);
                _idle_time++;
            }else if(_idle_time <= 1*60 + _dir*2){
                _idle_time++;
            }else{
                _idle_time = 0;
            }

            // If direction is valid
            if(_dir != Direction::NEUTRAL && _dir < 9){
                // Move if dir not obstructed
                if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && map_obstacle(x, y, NORTH)){          // UP
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                    set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
                }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, SOUTH)){  // DOWN
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                    set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
                }
                if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && map_obstacle(x, y, WEST)){  // LEFT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                    set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
                }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, EAST)){ // RIGHT
                    bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                    set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
                }
            }
            
        }
        
        if(_state == State::NORMAL && !is_attacking(40)){ walking_update(bn::sprite_items::bad_cat.tiles_item());}
    }
}

void BadCat::update(){
    if(on_screen(Common::Camera())){
        if(!_sprite.has_value()){
            load_graphics(bn::sprite_items::bad_cat, 8, 4);
        }

        if(Common::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Common::Player().sprite().z_order() - 1);}

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
            }else if(!is_attacking(40)){
                move();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Common::Player().alive()){
                if(Common::Player().get_state() == State::ATTACKING && Common::Player().get_hitbox().intersects(rect())){
                    got_hit(Common::Player().get_attack());
                    if(Common::Player().get_state() != State::HURTING){ Common::Player().set_state(State::NORMAL);}
                }
                if(get_state() == State::ATTACKING && get_hitbox().intersects(Common::Player().rect())){
                    Common::Player().got_hit(get_attack());
                    if(get_state() != State::HURTING){ set_state(State::NORMAL);}
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
    bn::fixed_point xyVector = jv::normalize(Common::Player().position() - position());
    int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
        
    // Player within range
    if(in_range(Common::Player().int_x(), Common::Player().int_y(), 20)){
        look_at(xyVector);
        attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(in_range(Common::Player().int_x(), Common::Player().int_y(), 46) && !is_attacking(40)){
        look_at(xyVector);
        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = this->x();
        bn::fixed target_y = this->y();
        if((xyVector.x() > 0 && map_obstacle(x, y, EAST)) || (xyVector.x() < 0 && map_obstacle(x, y, WEST))){
            target_x += xyVector.x()*_stats.speed;
        }
        if((xyVector.y() > 0 && map_obstacle(x, y, SOUTH)) || (xyVector.y() < 0 && map_obstacle(x, y, NORTH))){
            target_y += xyVector.y()*_stats.speed + 8;
        }

        set_position(target_x, target_y, 8);
    }

    // Random direction
    else{
        if(_idle_time == 0){
            _dir = Common::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        // If direction is valid
        if(_dir != Direction::NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && map_obstacle(x, y, NORTH)){          // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                set_position(this->x(), this->y() + 8 - _stats.speed*diagonal, 8); 
            }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, SOUTH)){  // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                set_position(this->x(), this->y() + 8 + _stats.speed*diagonal, 8);
            }
            if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && map_obstacle(x, y, WEST)){  // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                set_position(this->x() - _stats.speed*diagonal, this->y() + 8, 8);
            }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, EAST)){ // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                set_position(this->x() + _stats.speed*diagonal, this->y() + 8, 8);
            }
        }
        
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){ walking_update(bn::sprite_items::pale_tongue.tiles_item());}
}

void PaleTongue::update(){
    if(on_screen(Common::Camera())){
        if(!_sprite.has_value()){
            load_graphics(bn::sprite_items::pale_tongue, 8, 8);
        }

        if(Common::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Common::Player().sprite().z_order() - 1);}

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
            }else if(!is_attacking(40)){
                move();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Common::Player().alive()){
                if(Common::Player().get_state() == State::ATTACKING && Common::Player().get_hitbox().intersects(rect())){
                    got_hit(Common::Player().get_attack());
                    if(Common::Player().get_state() != State::HURTING){ Common::Player().set_state(State::NORMAL);}
                }
                if(get_state() == State::ATTACKING && get_hitbox().intersects(Common::Player().rect())){
                    Common::Player().got_hit(get_attack());
                    if(get_state() != State::HURTING){ set_state(State::NORMAL);}
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
    bn::fixed_point xyVector = jv::normalize(Common::Player().position() - bn::point(this->int_x(), this->int_y()));
    int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
        
    // Player within range
    if(in_range(Common::Player().int_x(), Common::Player().int_y(), 40)){
        look_at(xyVector);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = this->x();
        bn::fixed target_y = this->y();
        if((xyVector.x() < 0 && map_obstacle(x, y, EAST)) || (xyVector.x() > 0 && map_obstacle(x, y, WEST))){
            target_x -= xyVector.x()*_stats.speed;
        }
        if((xyVector.y() < 0 && map_obstacle(x, y, SOUTH)) || (xyVector.y() > 0 && map_obstacle(x, y, NORTH))){
            target_y -= xyVector.y()*_stats.speed;
        }

        set_position(target_x, target_y + 20, 20);
    }else if(in_range(Common::Player().int_x(), Common::Player().int_y(), 70)){
        look_at(xyVector);
        
        if(_idle_time == 0){
            attack();
            _idle_time++;
        }else if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }

    // Random direction
    else{
        if(_idle_time == 0){
            _dir = Common::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        // If direction is valid
        if(_dir != Direction::NEUTRAL && _dir < 9){
            // Move if dir not obstructed
            if((_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) && map_obstacle(x, y, NORTH)){          // UP
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST);
                set_position(this->x(), this->y() + 20 - _stats.speed*diagonal, 20); 
            }else if((_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, SOUTH)){  // DOWN
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST);
                set_position(this->x(), this->y() + 20 + _stats.speed*diagonal, 20);
            }
            if((_dir == Direction::WEST || _dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) && map_obstacle(x, y, WEST)){  // LEFT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST);
                set_position(this->x() - _stats.speed*diagonal, this->y() + 20, 20);
            }else if((_dir == Direction::EAST || _dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) && map_obstacle(x, y, EAST)){ // RIGHT
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST);
                set_position(this->x() + _stats.speed*diagonal, this->y() + 20, 20);
            }
        }
        
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){ walking_update(bn::sprite_items::pale_finger.tiles_item());}
}

void PaleFinger::attack(){
    if(!(_attack_cooldown + _idle_time)){
        _attack_cooldown = 60;
        set_animation(frames::Attack, bn::sprite_items::pale_finger.tiles_item(), 8);
        Common::create_projectile(this->int_x(), this->int_y() - 40);
    }
}

void PaleFinger::update(){
    if(on_screen(Common::Camera(), 32, 58)){
        if(!_sprite.has_value()){
            load_graphics(bn::sprite_items::pale_finger, 20, 8);
        }

        if(Common::Player().sprite().y() > _sprite->y() + 8){ _sprite->set_z_order(Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Common::Player().sprite().z_order() - 1);}

        if(alive()){
            attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(frames::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                move();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Common::Player().alive()){
                if(Common::Player().get_state() == State::ATTACKING && Common::Player().get_hitbox().intersects(rect())){
                    got_hit(Common::Player().get_attack());
                    if(Common::Player().get_state() != State::HURTING){ Common::Player().set_state(State::NORMAL);}
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
void NPC::update(jv::stairs& stairs, bool objective){
    if(on_screen(Common::Camera())){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(Common::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build();
            _animation = bn::create_sprite_animate_action_forever(_sprite.value(), 8,
                                    bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
        }

        if(Common::Player().sprite().y() > _sprite->y()){
            _sprite->set_z_order(Common::Player().sprite().z_order() + 1);
        }else{
            _sprite->set_z_order(Common::Player().sprite().z_order() - 1);
        }

        if(Common::Player().get_state() == State::NORMAL && !Common::Player().is_attacking()){
            // Dialog
            if(bn::keypad::a_pressed() && Common::Player().rect().intersects(rect()) && Common::Player()._interact_token){
                if(!objective){
                    jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
                }else{
                    jv::Dialog::init("Thanks for getting rid of the evil", "creatures! The stairs are open now!");
                    if(!stairs.isOpen){
                        stairs.set_open(true);
                    }
                }
                Common::Player().set_interact_token(false);
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