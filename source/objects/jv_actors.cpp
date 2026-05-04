#include "jv_actors.h"

#include "jv_math.h"
#include "jv_dialog.h"
#include "jv_stairs.h"
#include "jv_tiled_bg.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

#include "bn_sprite_items_cursor.h"

#ifdef DEV_ENABLED
    #include "bn_log.h"
#endif

namespace jv{
// ************** Actor *************
[[nodiscard]] bool Actor::on_screen(uint8_t halfWidth, uint8_t halfHeight) const {
    uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = y() > Global::cam_pos().y() - y_offset, down = y() < Global::cam_pos().y() + y_offset;
    bool left = x() > Global::cam_pos().x() - x_offset, right = x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}

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
    return Global::Map().cell(point_1) <= WTILES_COUNT && Global::Map().cell(point_2) <= WTILES_COUNT;
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
Player::Player(bn::point position):
    Actor(bn::rect(position.x(), position.y(), 16, 16)),
    _stats(basic_stats(1, 1, 5, bn::fixed(1.5))),
    _hitbox(bn::rect(position.x(), position.y(), 10, 10))
    {
        _hp = _stats.max_hp;
        bn::sprite_builder builder(bn::sprite_items::good_cat);
        builder.set_position(position.x(), position.y() - _sprite_y_offset());
        //builder.set_camera(Global::Camera()); // *** Do not set camera from Global before it is initialized ***
        builder.set_bg_priority(1);
        
        graphics.sprite = builder.release_build();
        graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 4, bn::sprite_items::good_cat.tiles_item(), animation::idle);
    }

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
    bool up_held    = bn::keypad::up_held(),
         down_held  = bn::keypad::down_held(),
         left_held  = bn::keypad::left_held(),
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
    if(alive()) [[likely]] {
        _interact_token = true;
        _prev_pos = position();
        
        _attack_update();
        if(get_state() == State::HURTING) [[unlikely]] {
            if(graphics.animation->done()) [[unlikely]] {
                set_state(State::NORMAL);
                graphics.set_animation(_dir, animation::Walk, bn::sprite_items::good_cat.tiles_item());
            }
        }else if(!is_attacking()) [[likely]] {
            _movement();
        }
        
        _moved = _prev_pos != position();

        if(!graphics.animation->done()) [[likely]] { graphics.animation->update();}

        // Combat
        if(bn::keypad::b_pressed()) [[unlikely]] {
            _start_attack();
        }
    }
}

// ************** Enemy *************
// ************* BadCat *************
BadCat::BadCat(bn::point position):
    Enemy(_stats.max_hp, position)
    {
        if(on_screen()){
            bn::sprite_builder builder(bn::sprite_items::bad_cat);
            builder.set_position(position.x(), position.y() - _sprite_y_offset());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 4, bn::sprite_items::bad_cat.tiles_item(), animation::idle);
        }
    }

void BadCat::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - position());
    
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
            graphics.set_animation(_dir, animation::Walk, bn::sprite_items::bad_cat.tiles_item());
        }
        _prev_dir = _dir;
    }
}

void BadCat::update(){
    if(on_screen()){
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
PaleTongue::PaleTongue(bn::point position):
    Enemy(_stats.max_hp, position)
    {
        if(on_screen()){
            bn::sprite_builder builder(bn::sprite_items::pale_tongue);
            builder.set_position(position.x(), position.y() - _sprite_y_offset());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 8, bn::sprite_items::pale_tongue.tiles_item(), animation::idle);
        }
}

void PaleTongue::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - position());
        
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
    if(on_screen()){
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
PaleFinger::PaleFinger(bn::point position):
    Enemy(_stats.max_hp, position)
    {
        if(on_screen(32, 58)){
            bn::sprite_builder builder(bn::sprite_items::pale_finger);
            builder.set_position(position.x(), position.y() - _sprite_y_offset());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 8, bn::sprite_items::pale_finger.tiles_item(), animation::idle);
        }
    }

void PaleFinger::_movement(){
    bn::fixed_point player_direction = jv::normalize(Global::Player().position() - bn::point(x(), y() - 8));
        
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
    if(on_screen(32, 58)){
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

// ************* NPCs **************
Cow::Cow(bn::point position):
    NPC(bn::rect(position.x(), position.y() + 8, 20, 20))
    {
        if(on_screen()){
            bn::sprite_builder builder(bn::sprite_items::cow);
            builder.set_position(position.x(), position.y() - _sprite_y_offset());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            constexpr uint16_t arr[4] = {0, 1, 2, 3};
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 8, bn::sprite_items::cow.tiles_item(), arr);
        }
    }

void Cow::update(){
    if(on_screen()){
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
                if(!Global::Stairs().isOpen){
                    jv::Dialog::init("Thanks for finding me!", "The stairs are open now!");
                    Global::Stairs().set_open(true);
                    Global::Tiled_Bg().init();
                }else{
                    jv::Dialog::init("Find the stairs! They're open!");
                }
                Global::Player().spend_interact_token();
            }
        }

        graphics.animation->update();
    }else{
        if(graphics.sprite.has_value()){
            graphics.reset();
        }
    }
    
}

Fox::Fox(bn::point position):
    NPC(bn::rect(position.x(), position.y() + 8, 20, 20))
    {
        if(on_screen()){
            bn::sprite_builder builder(bn::sprite_items::fox);
            builder.set_position(position.x(), position.y() - _sprite_y_offset());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            constexpr uint16_t arr[4] = {0, 1, 0, 2};
            graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), arr);
        }
    }

void Fox::update(){
    if(on_screen()){
        if(!graphics.sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::fox);
            builder.set_position(x(), y());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            graphics.sprite = builder.release_build();
            graphics.animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 0, 1, 0, 2);
        }

        if(Global::Player().graphics.y() > sprite().y()){
            sprite().set_z_order(Global::Player().graphics.z_order() + 1);
        }else{
            sprite().set_z_order(Global::Player().graphics.z_order() - 1);
        }
        
        if(Global::Player().get_state() == State::NORMAL && !Global::Player().is_attacking()) [[likely]] {
            // Dialog
            if(bn::keypad::a_pressed() && Global::Player().rect().intersects(rect()) && Global::Player().can_interact()){
                Global::Player().get_hitbox().set_position(this->graphics.x().floor_integer(), this->graphics.y().floor_integer());
                bn::fixed player_speed = Global::Player().get_speed()/2;
                bn::fixed_point player_target = this->position() + bn::point(0, 14);
                bn::fixed_point player_direction = jv::normalize(player_target - Global::Player().graphics.position());
                
                while(true){
                    Global::update();
                    bn::fixed target_x = Global::Player().graphics.x(), target_y = Global::Player().graphics.y();
                    if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
                        target_x += player_direction.x()*player_speed;
                    }
                    if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
                        target_y += player_direction.y()*player_speed;
                    }
                    Global::Player().set_position(target_x, target_y);

                    if(Global::Player().graphics.y() > sprite().y()){
                        sprite().set_z_order(Global::Player().graphics.z_order() + 1);
                    }else{
                        sprite().set_z_order(Global::Player().graphics.z_order() - 1);
                    }
                    
                    Global::Player().graphics.animation->update();
                    this->graphics.animation->update();

                    if(player_target.y() - Global::Player().graphics.y() < 0){
                        Global::Player().set_position(player_target);
                        Global::Player().graphics.set_animation(NORTH, animation::Walk, bn::sprite_items::good_cat.tiles_item());
                        Global::Player().graphics.animation->update();
                        break;
                    }
                    bn::core::update();
                }

                jv::Dialog::init("Kekeke. Hello there delver. I got", "something you might like. All you", "have to do is guess correctly.");
                
                this->graphics.animation = bn::create_sprite_animate_action_once(sprite(), 8, bn::sprite_items::fox.tiles_item(), 3, 4, 5, 6, 7);

                while(!graphics.animation->done()){
                    Global::Player().graphics.animation->update();
                    graphics.animation->update();
                    bn::core::update();
                }
                for(int stall = 0; stall < 30; stall++) bn::core::update();

                jv::Dialog::init("So tell me. Is it in your left or", "my right?.");
                this->graphics.animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 8, 9, 8, 10);

                {
                    bn::sprite_text_generator text_gen(common::variable_8x8_sprite_font);
                    bn::vector<bn::sprite_ptr, 9> txt_sprts;
                    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-10, 0);

                    text_gen.set_bg_priority(0);
                    text_gen.generate(-64, 0, "Your left     His right", txt_sprts);
                    cursor.set_bg_priority(0);

                    bn::core::update();

                    while(!bn::keypad::a_pressed()){
                        if(bn::keypad::right_pressed()){
                            if(cursor.x() < 0){
                                cursor.set_x(-cursor.x());
                                cursor.set_horizontal_flip(!cursor.horizontal_flip());
                            }
                        }else if(bn::keypad::left_pressed()){
                            if(cursor.x() > 0){
                                cursor.set_x(-cursor.x());
                                cursor.set_horizontal_flip(!cursor.horizontal_flip());
                            }
                        }

                        Global::Player().graphics.animation->update();
                        this->graphics.animation->update();
                        bn::core::update();
                    }
                }

                this->graphics.animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 7, 7, 7);
                jv::Dialog::init("Sorry pal. Wrong paw. Better luck", "next time ;)");
                this->graphics.animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 0, 1, 0, 2);

                Global::Player().spend_interact_token();
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