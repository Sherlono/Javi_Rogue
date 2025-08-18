#include "jv_actors.h"

#include "jv_math.h"
#include "jv_dialog.h"
#include "jv_stairs.h"
#include "jv_tiled_bg.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

#ifdef DEV_ENABLED
    #include "bn_log.h"
#endif

namespace jv{
// ************ Actor ************
[[nodiscard]] bool Actor::map_obstacle(const int x, const int y, const uint8_t direction){
    int target_x, target_y;
    switch(direction){
        case Direction::NORTH:
            target_x = x;
            target_y = y - 1;
            break;
        case Direction::SOUTH:
            target_x = x;
            target_y = y + 1;
            break;
        case Direction::WEST:
            target_x = x - 1;
            target_y = y;
            break;
        case Direction::EAST:
            target_x = x + 1;
            target_y = y;
            break;
        default:
            BN_ASSERT(false, "Invalid direction", direction);
            return false;
    }
    return Global::Map().cell(target_x, target_y) > 0 && Global::Map().cell(target_x, target_y) < WTILES_COUNT;
}

void Actor::load_graphics(const bn::sprite_item& item, int y_offset, int wait_frames){
    bn::sprite_builder builder(item);
    builder.set_position(this->int_x(), this->int_y() - y_offset);
    builder.set_camera(Global::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
    set_animation(animation::Walk, item.tiles_item(), wait_frames);
}

// ************ Player ************
bool Player::_enemy_obstacle(int x, int y, const uint8_t direction){
    switch(direction){
        case Actor::Direction::NORTH:{
            bn::point p(x, y - 8);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::SOUTH:{
            bn::point p(x, y + 8);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::WEST:{
            bn::point p(x - 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::EAST:{
            bn::point p(x + 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        default:
            BN_ASSERT(false, "Invalid direction", direction);
            break;
    }
    return true;
}

void Player::_movement(bool noClip){
    if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
        _dir = bn::keypad::up_held() + 2*bn::keypad::down_held() + 3*bn::keypad::left_held() + 6*bn::keypad::right_held();

        int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;

        // Move if dir not obstructed
        if(bn::keypad::up_held() && (noClip || (map_obstacle(x, y, NORTH) && _enemy_obstacle(int_x(), int_y(), NORTH)))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = this->y() - _stats.speed*diagonal;
            set_position(this->x(), target_y + 8, 8);
        }else if(bn::keypad::down_held() && (noClip || (map_obstacle(x, y, SOUTH) && _enemy_obstacle(int_x(), int_y(), SOUTH)))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = this->y() + _stats.speed*diagonal;
            set_position(this->x(), target_y + 8, 8);
        }
        if(bn::keypad::left_held() && (noClip || (map_obstacle(x, y, WEST) && _enemy_obstacle(int_x(), int_y(), WEST)))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = this->x() - _stats.speed*diagonal;
            set_position(target_x, this->y() + 8, 8);
        }else if(bn::keypad::right_held() && (noClip || (map_obstacle(x, y, EAST) && _enemy_obstacle(int_x(), int_y(), EAST)))){
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = this->x() + _stats.speed*diagonal;
            set_position(target_x, this->y() + 8, 8);
        }
        _hitbox.set_position(this->int_x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                            this->int_y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
    }

    if(_state == State::NORMAL){
        if(_prev_dir != _dir){
            set_animation(animation::Walk, bn::sprite_items::good_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void Player::update(bool noClip){
    #ifdef DEV_ENABLED
        if(bn::keypad::l_pressed()){
            int value = jv::Global::Map().cell(this->int_x()>>3, (this->int_y()+4)>>3);
            BN_LOG("x: ", this->int_x()>>3, " y: ", (this->int_y()+4)>>3, " Value: ", value);
        }
    #endif
    
    if(alive()){
        _interact_token = true;
        
        _attack_update();
        if(get_state() == State::HURTING){
            if(_animation->done()){
                set_state(State::NORMAL);
                set_animation(animation::Walk, bn::sprite_items::good_cat.tiles_item());
            }
        }else if(!is_attacking()){
            _movement(noClip);
        }
        if(!_animation->done()){ _animation->update();}

        // Combat
        if(bn::keypad::b_pressed()){
            _start_attack();
        }
    }
}

// ************ Enemy ************
/*[[nodiscard]] bool Enemy::on_screen(uint8_t halfWidth, uint8_t halfHeight) const {
    uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = this->int_y() > Global::cam_pos().y() - y_offset, down = this->int_y() < Global::cam_pos().y() + y_offset;
    bool left = this->int_x() > Global::cam_pos().x() - x_offset, right = this->int_x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}*/

// ************* BadCat *************
void BadCat::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - position());
    int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
    
    // Player within range
    if(in_range(Global::Player().int_position(), 18)){
        look_at(player_direction);
        _start_attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(in_range(Global::Player().int_position(), 46)){
        look_at(player_direction);
        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = this->x();
        bn::fixed target_y = this->y();
        if((player_direction.x() > 0 && map_obstacle(x, y, EAST)) || (player_direction.x() < 0 && map_obstacle(x, y, WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && map_obstacle(x, y, SOUTH)) || (player_direction.y() < 0 && map_obstacle(x, y, NORTH))){
            target_y += player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y + SPRTYOFFSET, SPRTYOFFSET);
    }

    // Random direction
    else{
        if(_idle_time == 0){
            _dir = Global::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        displace(x, y, SPRTYOFFSET, _stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            set_animation(animation::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void BadCat::update(){
    if(on_screen(Global::Camera())){
        if(!_sprite.has_value()){
            int wait_frames = 4;
            load_graphics(bn::sprite_items::bad_cat, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(Global::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Global::Player().sprite().z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::bad_cat.tiles_item());
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
                bool attack_connected_player = get_state() == State::ATTACKING && in_range(Global::Player().int_position(), 25);
                if(attack_connected_player){
                    Global::Player().got_hit(get_attack());
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
void PaleTongue::_movement(){
    
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - position());
    int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
        
    // Player within range
    if(in_range(Global::Player().int_position(), 20)){
        look_at(player_direction);
        _start_attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(in_range(Global::Player().int_position(), 46) && !is_attacking(40)){
        look_at(player_direction);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = this->x(), target_y = this->y();
        
        if((player_direction.x() > 0 && map_obstacle(x, y, EAST)) || (player_direction.x() < 0 && map_obstacle(x, y, WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && map_obstacle(x, y, SOUTH)) || (player_direction.y() < 0 && map_obstacle(x, y, NORTH))){
            target_y += player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y + SPRTYOFFSET, SPRTYOFFSET);
    }

    // Random direction
    else{
        if(_idle_time == 0){
            _dir = Global::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        displace(x, y, SPRTYOFFSET, _stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            set_animation(animation::Walk, bn::sprite_items::pale_tongue.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void PaleTongue::update(){
    if(on_screen(Global::Camera())){
        if(!_sprite.has_value()){
            int wait_frames = 8;
            load_graphics(bn::sprite_items::pale_tongue, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().sprite().y() > _sprite->y()){ _sprite->set_z_order(Global::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Global::Player().sprite().z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
                bool attack_connected_player = get_state() == State::ATTACKING && in_range(Global::Player().int_position(), 25);
                if(attack_connected_player){
                    Global::Player().got_hit(get_attack());
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
void PaleFinger::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - bn::point(this->int_x(), this->int_y() - 8));
    int x = this->int_x()>>3, y = (this->int_y() + 4)>>3;
        
    // Player within range
    if(in_range(Global::Player().int_position(), 40)){
        look_at(player_direction);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = this->x(), target_y = this->y();

        if((player_direction.x() < 0 && map_obstacle(x, y, EAST)) || (player_direction.x() > 0 && map_obstacle(x, y, WEST))){
            target_x -= player_direction.x()*_stats.speed;
        }
        if((player_direction.y() < 0 && map_obstacle(x, y, SOUTH)) || (player_direction.y() > 0 && map_obstacle(x, y, NORTH))){
            target_y -= player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y + SPRTYOFFSET, SPRTYOFFSET);

    }else if(in_range(Global::Player().int_position(), 70)){
        look_at(player_direction);
        
        if(_idle_time == 0){
            _start_attack();
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
            _dir = Global::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + _dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        // If direction is valid
        displace(x, y, SPRTYOFFSET, _stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            set_animation(animation::Walk, bn::sprite_items::pale_finger.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void PaleFinger::_start_attack(){
    if(!(_attack_cooldown + _idle_time)){
        _attack_cooldown = 60;
        set_animation(animation::Id::Attack, bn::sprite_items::pale_finger.tiles_item(), 8);
        Global::create_projectile(this->int_x(), this->int_y() - 40, Projectile::IDs::ENERGYORB);
    }
}

void PaleFinger::update(){
    if(on_screen(Global::Camera(), 32, 58)){
        if(!_sprite.has_value()){
            int wait_frames = 8;
            load_graphics(bn::sprite_items::pale_finger, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().sprite().y() > _sprite->y() + 8){ _sprite->set_z_order(Global::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Global::Player().sprite().z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(_animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!_animation->done()){ _animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
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
void NPC::update(jv::stairs& stairs, tiled_bg& bg, bool objective){
    if(on_screen(Global::Camera())){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(this->int_x(), this->int_y() - 8);
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build();
            _animation = bn::create_sprite_animate_action_forever(_sprite.value(), 8,
                                    bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
        }

        if(Global::Player().sprite().y() > _sprite->y()){
            _sprite->set_z_order(Global::Player().sprite().z_order() + 1);
        }else{
            _sprite->set_z_order(Global::Player().sprite().z_order() - 1);
        }

        if(Global::Player().get_state() == State::NORMAL && !Global::Player().is_attacking()){
            // Dialog
            if(bn::keypad::a_pressed() && Global::Player().rect().intersects(rect()) && Global::Player().can_interact()){
                if(!objective){
                    jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
                }else{
                    jv::Dialog::init("Thanks for getting rid of the evil", "creatures! The stairs are open now!");
                    if(!stairs.isOpen){
                        stairs.set_open(true);
                        bg.init();
                    }
                }
                Global::Player().set_interact_token(false);
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