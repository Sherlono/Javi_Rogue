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
// ************** Actor *************
[[nodiscard]] bool Actor::_map_obstacle(const uint8_t direction){
    bn::point point_1, point_2;
    int aux;
    switch(direction){
        case NORTH:
            aux = ((y() + 3)>>3) - 1;
            point_1.set_x((x() - 5)>>3);
            point_1.set_y(aux);
            point_2.set_x((x() + 5)>>3);
            point_2.set_y(aux);
            break;
        case SOUTH:
            aux = ((y() + 3)>>3) + 1;
            point_1.set_x((x() - 5)>>3);
            point_1.set_y(aux);
            point_2.set_x((x() + 5)>>3);
            point_2.set_y(aux);
            break;
        case WEST:
            aux = (x()>>3) - 1;
            point_1.set_x(aux);
            point_1.set_y(y()>>3);
            point_2.set_x(aux);
            point_2.set_y((y() + 6)>>3);
            break;
        case EAST:
            aux = (x()>>3) + 1;
            point_1.set_x(aux);
            point_1.set_y(y()>>3);
            point_2.set_x(aux);
            point_2.set_y((y() + 6)>>3);
            break;
        default:
            BN_ERROR("Invalid direction: ", direction);
            return false;
    }
    return Global::Tiled_Bg().game_map().cell(point_1) <= WTILES_COUNT && Global::Tiled_Bg().game_map().cell(point_2) <= WTILES_COUNT;
}

void Actor::_load_graphics(const bn::sprite_item& item, int wait_frames){
    bn::sprite_builder builder(item);
    builder.set_position(x(), y() - _sprite_y_offset());
    builder.set_camera(Global::Camera());
    builder.set_bg_priority(1);
    
    graphics.sprite = builder.release_build();
    graphics.set_animation(_dir, animation::Walk, item.tiles_item(), wait_frames);
}

// ************* Player *************
bool Player::_enemy_obstacle(const int x, const int  y, const uint8_t direction){
    switch(direction){
        case NORTH:{
            bn::point p(x, y - 6);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case SOUTH:{
            bn::point p(x, y + 6);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case WEST:{
            bn::point p(x - 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case EAST:{
            bn::point p(x + 8, y);
            for(int i = 0; i < _enemies_ref->size(); i++){
                if(_enemies_ref->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        default:
            BN_ERROR("Invalid direction: ", direction);
            break;
    }
    return true;
}

void Player::_movement(){
    bool up_held = bn::keypad::up_held(),
         down_held = bn::keypad::down_held(),
         left_held = bn::keypad::left_held(),
         right_held = bn::keypad::right_held();

    if(up_held || down_held || left_held || right_held){
        _dir = 1*up_held + 2*down_held + 3*left_held + 6*right_held;

        // Move if dir not obstructed
        if(up_held && (noClip || (_map_obstacle(NORTH) && _enemy_obstacle(x(), y(), NORTH)))){         // Up
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = graphics.y() - _stats.speed*diagonal;
            set_position(graphics.x(), target_y);
        }else if(down_held && (noClip || (_map_obstacle(SOUTH) && _enemy_obstacle(x(), y(), SOUTH)))){ // Down
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(left_held || right_held);
            bn::fixed target_y = graphics.y() + _stats.speed*diagonal;
            set_position(graphics.x(), target_y);
        }
        if(left_held && (noClip || (_map_obstacle(WEST) && _enemy_obstacle(x(), y(), WEST)))){         // Left
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = graphics.x() - _stats.speed*diagonal;
            set_position(target_x, graphics.y());
        }else if(right_held && (noClip || (_map_obstacle(EAST) && _enemy_obstacle(x(), y(), EAST)))){  // Right
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(up_held || down_held);
            bn::fixed target_x = graphics.x() + _stats.speed*diagonal;
            set_position(target_x, graphics.y());
        }
        _hitbox.set_position(x() - 10*(_dir == NORTHWEST || _dir == SOUTHWEST) + 10*(_dir == NORTHEAST || _dir == SOUTHEAST) - 16*(_dir == WEST) + 16*(_dir == EAST),
                            y() - 10*(_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) + 10*(_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST));
    }
    
    if(_state == State::NORMAL){
        if(_prev_dir != _dir){
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::good_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void Player::update(){
    #ifdef DEV_ENABLED
    if(bn::keypad::l_pressed()) [[unlikely]] {
        int value = jv::Global::Tiled_Bg().game_map().cell(x()>>3, (y()+4)>>3);
        BN_LOG("x: ", x()>>3, " y: ", (y()+4)>>3, " Value: ", value);
    }
    #endif
    
    if(alive()) [[likely]] {
        _interact_token = true;
        
        _attack_update();
        if(get_state() == State::HURTING) [[unlikely]] {
            if(graphics.animation->done()) [[unlikely]] {
                set_state(State::NORMAL);
                graphics.set_animation(_dir, animation::Walk, bn::sprite_items::good_cat.tiles_item());
            }
        }else if(!is_attacking()) [[likely]] {
            _movement();
        }
        if(!graphics.animation->done()) [[likely]] { graphics.animation->update();}

        // Combat
        if(bn::keypad::b_pressed()) [[unlikely]] {
            _start_attack();
        }
    }
}

// ************** Enemy *************
/*[[nodiscard]] bool Enemy::on_screen(uint8_t halfWidth, uint8_t halfHeight) const {
    uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = y() > Global::cam_pos().y() - y_offset, down = y() < Global::cam_pos().y() + y_offset;
    bool left = x() > Global::cam_pos().x() - x_offset, right = x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}*/

// ************* BadCat *************
void BadCat::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - graphics.position());
    
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
        if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
            target_y += player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y);
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

        displace(_stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void BadCat::update(){
    if(on_screen(Global::Camera())){
        if(!graphics.sprite.has_value()){
            _load_graphics(bn::sprite_items::bad_cat, 4);
        }

        if(Global::Player().graphics.y() > sprite().y()){ sprite().set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ sprite().set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()) [[likely]] {
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    graphics.set_animation(_dir, animation::Walk, bn::sprite_items::bad_cat.tiles_item());
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

// *********** PaleTongue ***********
void PaleTongue::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - graphics.position());
        
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
        
        if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
            target_x += player_direction.x()*_stats.speed;
        }
        if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
            target_y += player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y);
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

        displace(_stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_tongue.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void PaleTongue::update(){
    if(on_screen(Global::Camera())){
        if(!graphics.sprite.has_value()){
            _load_graphics(bn::sprite_items::pale_tongue, 8);
        }

        if(Global::Player().graphics.y() > sprite().y()){ sprite().set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ sprite().set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()) [[likely]] {
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING) [[unlikely]] {
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_tongue.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!graphics.animation->done()) [[likely]] { graphics.animation->update();}
            
            // Combat
            if(Global::Player().alive()) [[likely]] {
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

// *********** PaleFinger **********
void PaleFinger::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().graphics.position() - bn::point(x(), y() - 8));
        
    // Player within range
    if(in_range(Global::Player().position(), 40)){
        look_at(player_direction);

        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = graphics.x(), target_y = graphics.y();

        if((player_direction.x() < 0 && _map_obstacle(EAST)) || (player_direction.x() > 0 && _map_obstacle(WEST))){
            target_x -= player_direction.x()*_stats.speed;
        }
        if((player_direction.y() < 0 && _map_obstacle(SOUTH)) || (player_direction.y() > 0 && _map_obstacle(NORTH))){
            target_y -= player_direction.y()*_stats.speed;
        }

        set_position(target_x, target_y);

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
        displace(_stats.speed);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_finger.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void PaleFinger::_start_attack(){
    if(!(_attack_cooldown + _idle_time)){
        _attack_cooldown = 60;
        graphics.set_animation(_dir, animation::Id::Attack, bn::sprite_items::pale_finger.tiles_item(), 8);
        Global::create_projectile(x(), y() - 40, Projectile::IDs::ENERGYORB);
    }
}

void PaleFinger::update(){
    if(on_screen(Global::Camera(), 32, 58)){
        if(!graphics.sprite.has_value()){
            _load_graphics(bn::sprite_items::pale_finger, 8);
        }

        if(Global::Player().graphics.y() > sprite().y() + 8){ sprite().set_z_order(Global::Player().graphics.z_order() + 1);}
        else{ sprite().set_z_order(Global::Player().graphics.z_order() - 1);}

        if(alive()){
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING) [[unlikely]] {
                if(graphics.animation->done()){
                    set_state(State::NORMAL);
                    if(!_idle_time){
                        _dir = 0;
                    }
                    graphics.set_animation(_dir, animation::Walk, bn::sprite_items::pale_finger.tiles_item(), 8);
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            if(!graphics.animation->done()) [[likely]] { graphics.animation->update();}
            
            // Combat
            if(Global::Player().alive()) [[likely]] {
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
void NPC::update(bool objective){
    if(on_screen(Global::Camera())){
        if(!graphics.sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(x(), y());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            graphics.animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::cow.tiles_item(), 0, 1, 2, 3);
        }

        if(Global::Player().graphics.y() > sprite().y()){
            sprite().set_z_order(Global::Player().graphics.z_order() + 1);
        }else{
            sprite().set_z_order(Global::Player().graphics.z_order() - 1);
        }

        if(Global::Player().get_state() == State::NORMAL && !Global::Player().is_attacking()) [[likely]] {
            // Dialog
            if(bn::keypad::a_pressed() && Global::Player().rect().intersects(rect()) && Global::Player().can_interact()){
                if(!objective){
                    jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
                }else if(!Global::Stairs().isOpen){
                    jv::Dialog::init("Thanks for finding me!", "The stairs are open now!");
                    Global::Stairs().set_open(true);
                    Global::Tiled_Bg().init();
                }else{
                    jv::Dialog::init("Find the stairs! They're open!");
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