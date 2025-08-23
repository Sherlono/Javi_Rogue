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
[[nodiscard]] bool Actor::map_obstacle(const int  x, const int  y, const uint8_t direction){
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
    builder.set_position(x(), y() - y_offset);
    builder.set_camera(Global::Camera());
    builder.set_bg_priority(1);
    
    graphics.sprite = builder.release_build();
    set_animation(animation::Walk, item.tiles_item(), wait_frames);
}

// ************ Player ************
bool Player::_enemy_obstacle(const int x, const int  y, const uint8_t direction){
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

        int coll_x = x()>>3, coll_y = (y() + 4)>>3;

        // Move if dir not obstructed
        if(bn::keypad::up_held() && (noClip || (map_obstacle(coll_x, coll_y, NORTH) && _enemy_obstacle(x(), y(), NORTH)))){         // Up
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = graphics.y() - _stats.speed*diagonal;
            set_position(graphics.x(), target_y + 8, 8);
        }else if(bn::keypad::down_held() && (noClip || (map_obstacle(coll_x, coll_y, SOUTH) && _enemy_obstacle(x(), y(), SOUTH)))){ // Down
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
            bn::fixed target_y = graphics.y() + _stats.speed*diagonal;
            set_position(graphics.x(), target_y + 8, 8);
        }
        if(bn::keypad::left_held() && (noClip || (map_obstacle(coll_x, coll_y, WEST) && _enemy_obstacle(x(), y(), WEST)))){         // Left
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = graphics.x() - _stats.speed*diagonal;
            set_position(target_x, graphics.y() + 8, 8);
        }else if(bn::keypad::right_held() && (noClip || (map_obstacle(coll_x, coll_y, EAST) && _enemy_obstacle(x(), y(), EAST)))){  // Right
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
            bn::fixed target_x = graphics.x() + _stats.speed*diagonal;
            set_position(target_x, graphics.y() + 8, 8);
        }
        _hitbox.set_position(x() - 10*(_dir == Direction::NORTHWEST || _dir == Direction::SOUTHWEST) + 10*(_dir == Direction::NORTHEAST || _dir == Direction::SOUTHEAST) - 16*(_dir == Direction::WEST) + 16*(_dir == Direction::EAST),
                            y() - 10*(_dir == Direction::NORTH || _dir == Direction::NORTHWEST || _dir == Direction::NORTHEAST) + 10*(_dir == Direction::SOUTH || _dir == Direction::SOUTHWEST || _dir == Direction::SOUTHEAST));
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
            int value = jv::Global::Map().cell(x()>>3, (y()+4)>>3);
            BN_LOG("x: ", x()>>3, " y: ", (y()+4)>>3, " Value: ", value);
        }
    #endif
    
    if(alive()){
        _interact_token = true;
        
        _attack_update();
        if(get_state() == State::HURTING){
            if(graphics.animation->done()){
                set_state(State::NORMAL);
                set_animation(animation::Walk, bn::sprite_items::good_cat.tiles_item());
            }
        }else if(!is_attacking()){
            _movement(noClip);
        }
        if(!graphics.animation->done()){ graphics.animation->update();}

        // Combat
        if(bn::keypad::b_pressed()){
            _start_attack();
        }
    }
}

// ************ Enemy ************
/*[[nodiscard]] bool Enemy::on_screen(uint8_t halfWidth, uint8_t halfHeight) const {
    uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = y() > Global::cam_pos().y() - y_offset, down = y() < Global::cam_pos().y() + y_offset;
    bool left = x() > Global::cam_pos().x() - x_offset, right = x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}*/

// ************* BadCat *************
void BadCat::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - graphics.position());
    int coll_x = x()>>3, coll_y = (y() + 4)>>3;
    
    // Player within range
    if(in_range(Global::Player().position(), 18)){
        look_at(player_direction);
        _start_attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(in_range(Global::Player().position(), 46)){
        look_at(player_direction);
        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = graphics.x();
        bn::fixed target_y = graphics.y();
        if((player_direction.x() > 0 && map_obstacle(coll_x, coll_y, EAST)) || (player_direction.x() < 0 && map_obstacle(coll_x, coll_y, WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && map_obstacle(coll_x, coll_y, SOUTH)) || (player_direction.y() < 0 && map_obstacle(coll_x, coll_y, NORTH))){
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

        displace(coll_x, coll_y, SPRTYOFFSET, _stats.speed);
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
        if(!graphics.sprite.has_value()){
            int wait_frames = 4;
            load_graphics(bn::sprite_items::bad_cat, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().graphics.y() > graphics.sprite->y()){ graphics.sprite->set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ graphics.sprite->set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::bad_cat.tiles_item());
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!graphics.animation->done()){ graphics.animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
                bool attack_connected_player = get_state() == State::ATTACKING && in_range(Global::Player().position(), 25);
                if(attack_connected_player){
                    Global::Player().got_hit(get_attack());
                }
            }
        }
    }else{
        if(graphics.sprite.has_value()){
            graphics.reset();
        }
    }
}

// ************* PaleTongue *************
void PaleTongue::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - graphics.position());
    int coll_x = x()>>3, coll_y = (y() + 4)>>3;
        
    // Player within range
    if(in_range(Global::Player().position(), 20)){
        look_at(player_direction);
        _start_attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(in_range(Global::Player().position(), 46) && !is_attacking(40)){
        look_at(player_direction);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = graphics.x(), target_y = graphics.y();
        
        if((player_direction.x() > 0 && map_obstacle(coll_x, coll_y, EAST)) || (player_direction.x() < 0 && map_obstacle(coll_x, coll_y, WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && map_obstacle(coll_x, coll_y, SOUTH)) || (player_direction.y() < 0 && map_obstacle(coll_x, coll_y, NORTH))){
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

        displace(coll_x, coll_y, SPRTYOFFSET, _stats.speed);
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
        if(!graphics.sprite.has_value()){
            int wait_frames = 8;
            load_graphics(bn::sprite_items::pale_tongue, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().graphics.y() > graphics.sprite->y()){ graphics.sprite->set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ graphics.sprite->set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!graphics.animation->done()){ graphics.animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
                bool attack_connected_player = get_state() == State::ATTACKING && in_range(Global::Player().position(), 25);
                if(attack_connected_player){
                    Global::Player().got_hit(get_attack());
                }
            }
        }
    }else{
        if(graphics.sprite.has_value()){
            graphics.reset();
        }
    }
}

// ************* PaleFinger *************
void PaleFinger::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - bn::point(x(), y() - 8));
    int coll_x = x()>>3, coll_y = (y() + 4)>>3;
        
    // Player within range
    if(in_range(Global::Player().position(), 40)){
        look_at(player_direction);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = graphics.x(), target_y = graphics.y();

        if((player_direction.x() < 0 && map_obstacle(coll_x, coll_y, EAST)) || (player_direction.x() > 0 && map_obstacle(coll_x, coll_y, WEST))){
            target_x -= player_direction.x()*_stats.speed;
        }
        if((player_direction.y() < 0 && map_obstacle(coll_x, coll_y, SOUTH)) || (player_direction.y() > 0 && map_obstacle(coll_x, coll_y, NORTH))){
            target_y -= player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y + SPRTYOFFSET, SPRTYOFFSET);

    }else if(in_range(Global::Player().position(), 70)){
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
        displace(coll_x, coll_y, SPRTYOFFSET, _stats.speed);
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
        Global::create_projectile(x(), y() - 40, Projectile::IDs::ENERGYORB);
    }
}

void PaleFinger::update(){
    if(on_screen(Global::Camera(), 32, 58)){
        if(!graphics.sprite.has_value()){
            int wait_frames = 8;
            load_graphics(bn::sprite_items::pale_finger, SPRTYOFFSET, wait_frames);
        }

        if(Global::Player().graphics.y() > graphics.sprite->y() + 8){ graphics.sprite->set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ graphics.sprite->set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    set_animation(animation::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!graphics.animation->done()){ graphics.animation->update();}
            
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
            }
        }
    }else{
        if(graphics.sprite.has_value()){
            graphics.reset();
        }
    }
}

// ************** NPC **************
void NPC::update(jv::stairs& stairs, tiled_bg& bg, bool objective){
    if(on_screen(Global::Camera())){
        if(!graphics.sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(x(), y() - 8);
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            graphics.animation = bn::create_sprite_animate_action_forever(graphics.sprite.value(), 8,
                                    bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
        }

        if(Global::Player().graphics.y() > graphics.sprite->y()){
            graphics.sprite->set_z_order(Global::Player().graphics.z_order() + 1);
        }else{
            graphics.sprite->set_z_order(Global::Player().graphics.z_order() - 1);
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

        graphics.animation->update();
    }else{
        if(graphics.sprite.has_value()){
            graphics.reset();
        }
    }
    
}
}