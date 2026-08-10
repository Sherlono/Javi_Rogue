#include "jv_actors.h"

#include "bn_array.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_map_classes.h"

#ifdef DEV_ENABLED
    #include "bn_log.h"
#endif

namespace jv{
// ************** Actor *************
Actor::Actor(const uint8_t actor_id, const bn::point p):
    id(actor_id), graphics_key(Global::Graphics_Manager().front_key()),
    _rect(bn::rect(p.x(), p.y(), Actor_data::meta[id].rect_shape.width(), Actor_data::meta[id].rect_shape.height()))
    {
        //Global::Graphics_Manager().create_sprite(actor_id, animation::Id::Walk, p);
    }

void Actor::set_position(const bn::fixed x, const bn::fixed y){
    sprite().set_position(x, y);
    _rect.set_position(x.floor_integer(), y.floor_integer() + Actor_data::meta[id].sprt_y_offset);
}
void Actor::set_position(const bn::fixed_point point){
    sprite().set_position(point.x(), point.y());
    _rect.set_position(point.x().floor_integer(), point.y().floor_integer() + Actor_data::meta[id].sprt_y_offset);
}

bool Actor::Graphics::horizontal_flip = false;
animation::Id Actor::Graphics::action = animation::Id::Walk;
uint16_t Actor::Graphics::sprite_item_id = 0;
bn::fixed_point Actor::Graphics::sprite_position = bn::fixed_point(0, 0);
animation_type Actor::Graphics::frames = animation::Walk_do[0];

[[nodiscard]] bn::sprite_ptr& Actor::sprite() {
    return Global::Graphics_Manager()[graphics_key];
}
void Actor::set_visible(bool visible){
    Global::Graphics_Manager()[graphics_key].set_visible(visible);
}
void Actor::set_camera(bn::camera_ptr& c){
    Global::Graphics_Manager()[graphics_key].set_camera(c);
}

[[nodiscard]] bool Actor::is_on_screen() const {
    uint8_t x_offset = 120 + Actor_data::meta[id].half_size.width(), y_offset = Actor_data::meta[id].half_size.height() + 80 + Actor_data::meta[id].sprt_y_offset;
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

// Creates graphics of the respective id, direction and action in graphics manager
void Actor::load_graphics(const animation::Id action){
    BN_ASSERT(!has_graphics, "");
    Graphics::configure_animation(id, _dir, action, bn::point(x(), y() - Actor_data::meta[id].sprt_y_offset));
    Global::Graphics_Manager().create_sprite();
    has_graphics = true;
}


// ************** Actor::Graphics *************

// Default Graphics constructor
Actor::Graphics::Graphics():
    bn::sprite_ptr(Actor_data::data[sprite_item_id].sprite_item_ref.create_sprite(sprite_position, frames[0])),
    animation(create_animation())
    {
        set_camera(Global::Camera());
        set_horizontal_flip(horizontal_flip);
        set_bg_priority(1);
    }

// Decides animation frames and horizontal flip based on direction and action performed
void Actor::Graphics::configure_animation(const uint8_t actor_id, const uint8_t dir, const animation::Id act, const bn::fixed_point pos){
    Actor::Graphics::sprite_item_id = actor_id;
    Actor::Graphics::sprite_position = pos;
    Actor::Graphics::action = act;

    switch(Actor::Graphics::action){
        case animation::Id::Walk:{
            if(dir == NORTH || dir == NORTHWEST || dir == NORTHEAST){        // UP
                horizontal_flip = false;
                frames = animation::Walk_up[Actor_data::meta[actor_id].anim_set];
            }else if(dir == SOUTH || dir == SOUTHWEST || dir == SOUTHEAST){  // DOWN
                horizontal_flip = false;
                frames = animation::Walk_do[Actor_data::meta[actor_id].anim_set];
            }else if(dir == WEST){                            // LEFT
                horizontal_flip = true;
                frames = animation::Walk_ho[Actor_data::meta[actor_id].anim_set];
            }else if(dir == EAST){                            // RIGHT
                horizontal_flip = false;
                frames = animation::Walk_ho[Actor_data::meta[actor_id].anim_set];
            }else{
                horizontal_flip = false;
                frames = animation::idle[Actor_data::meta[actor_id].anim_set];
                break;
            }
            break;
        }
        case animation::Id::Attack:{
            if(dir == NORTH || dir == NORTHWEST || dir == NORTHEAST){        // UP
                horizontal_flip = false;
                frames = animation::Attack_up[Actor_data::meta[actor_id].anim_set];
            }else if(dir == SOUTH || dir == SOUTHWEST || dir == SOUTHEAST){  // DOWN
                horizontal_flip = false;
                frames = animation::Attack_do[Actor_data::meta[actor_id].anim_set];
            }else if(dir == WEST){                            // LEFT
                horizontal_flip = true;
                frames = animation::Attack_ho[Actor_data::meta[actor_id].anim_set];
            }else if(dir == EAST){                            // RIGHT
                horizontal_flip = false;
                frames = animation::Attack_ho[Actor_data::meta[actor_id].anim_set];
            }else{
                horizontal_flip = false;
                frames = animation::idle[Actor_data::meta[actor_id].anim_set];
                break;
            }
            break;
        }
        case animation::Id::Idle:{
            horizontal_flip = false;
            frames = animation::idle[Actor_data::meta[actor_id].anim_set];
            break;
        }
        default:{
            horizontal_flip = false;
            frames = animation::Walk_do[Actor_data::meta[actor_id].anim_set];
            break;
        }
    }
}

// Releases animation_action based on current Graphics configuration and action performed
bn::sprite_animate_action<animation::MAX_FRAMES> Actor::Graphics::create_animation(){
    if(action != animation::Id::Attack){
        return bn::sprite_animate_action<animation::MAX_FRAMES>::forever(*this,
                                                                         Actor_data::data[sprite_item_id].wait_frames,
                                                                         Actor_data::data[sprite_item_id].sprite_item_ref.tiles_item(),
                                                                         frames);
    }
    else {
        return bn::sprite_animate_action<animation::MAX_FRAMES>::once(*this,
                                                                      Actor_data::data[sprite_item_id].wait_frames,
                                                                      Actor_data::data[sprite_item_id].sprite_item_ref.tiles_item(),
                                                                      frames);
    }
}


// ************** Enemy *************
Enemy::~Enemy() = default;

void Enemy::got_hit(int attack){
    _attack_cooldown = 0;
    _prev_attack_cooldown = 0;
    const uint8_t dmg = attack/Actor_data::stats[id].defense;
    hp = dmg > hp ? 0 : hp - dmg;
    if(hp > 0) [[likely]] {
        _state = State::HURTING;
        sprite().set_horizontal_flip(_dir == WEST);
        Global::Graphics_Manager()[graphics_key].animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(sprite(), 8, Actor_data::data[id].sprite_item_ref.tiles_item(), animation::hurt);
    }else{
        sprite().set_horizontal_flip(false);
        _state = State::DEAD;
    }
}

void Enemy::_start_attack(){
    if(!_attack_cooldown){
        _attack_cooldown = Actor_data::meta[id].atk_end_cooldown;
        Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Attack, position());
    }
}
void Enemy::_attack_update(){
        _prev_attack_cooldown = _attack_cooldown;
        if(_attack_cooldown) _attack_cooldown--;
        if(_state != State::HURTING){
            _state = State::NORMAL;
            if(_attack_cooldown == Actor_data::meta[id].atk_end_cooldown){ _state = State::ATTACKING;}
        }
        if(attack_ended(Actor_data::meta[id].atk_end_cooldown)){
            bn::fixed_point player_direction = Global::Player().normalized_vector(position());
            look_at(player_direction);

            Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Walk, position());
        }
    }

void Enemy::_simple_fsm_update(){
    if(get_state() == State::HURTING){
        if(Global::Graphics_Manager()[graphics_key].animation.done()){
            set_state(State::NORMAL);
            if(!_idle_time) _dir = NEUTRAL;
            Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Walk, position());
        }
    }else if(!is_attacking(40)){
        _movement();
    }
    // Combat
    if(Global::Player().alive()){
        bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
        if(player_attack_connected){
            got_hit(Global::Player().get_attack());
        }
        bool attack_connected_player = get_state() == State::ATTACKING && is_in_range(Global::Player().position(), 25);
        if(attack_connected_player){
            Global::Player().got_hit(Global::Player().get_defense());
        }
    }

    if(!Global::Graphics_Manager()[graphics_key].animation.done()){ Global::Graphics_Manager()[graphics_key].animation.update();}
    _prev_dir = _dir;
}

void Enemy::_movement(){
    bn::fixed_point player_direction = Global::Player().normalized_vector(position());
    
    // Player within range
    if(is_in_range(Global::Player().position(), Actor_data::meta[id].attack_range)){
        look_at(player_direction);
        _start_attack();
        
        if(_idle_time <= 2*60){
            _idle_time++;
        }else{
            _idle_time = 0;
        }
    }else if(is_in_range(Global::Player().position(), Actor_data::meta[id].view_range)){
        look_at(player_direction);
        if(_idle_time <= 2*60){
            _idle_time++;
        }
        bn::fixed target_x = Global::Graphics_Manager()[graphics_key].x(), target_y = Global::Graphics_Manager()[graphics_key].y();
        if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
            target_x += player_direction.x()*Actor_data::stats[id].speed;
        }
        if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
            target_y += player_direction.y()*Actor_data::stats[id].speed;
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

        _displace(bn::fixed(0.5));
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Walk, position());
        }
    }
}

void Enemy::_displace(const bn::fixed speed){
    // If direction is valid
    if(_dir != NEUTRAL && _dir < 9){
        // Move if dir not obstructed
        if((_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) && _map_obstacle( NORTH)){          // UP
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == NORTHEAST);
            set_position(Global::Graphics_Manager()[graphics_key].x(), Global::Graphics_Manager()[graphics_key].y() - speed*diagonal); 
        }else if((_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST) && _map_obstacle(SOUTH)){  // DOWN
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == SOUTHWEST || _dir == SOUTHEAST);
            set_position(Global::Graphics_Manager()[graphics_key].x(), Global::Graphics_Manager()[graphics_key].y() + speed*diagonal);
        }
        if((_dir == WEST || _dir == NORTHWEST || _dir == SOUTHWEST) && _map_obstacle(WEST)){  // LEFT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == SOUTHWEST);
            set_position(Global::Graphics_Manager()[graphics_key].x() - speed*diagonal, Global::Graphics_Manager()[graphics_key].y());
        }else if((_dir == EAST || _dir == NORTHEAST || _dir == SOUTHEAST) && _map_obstacle(EAST)){ // RIGHT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHEAST || _dir == SOUTHEAST);
            set_position(Global::Graphics_Manager()[graphics_key].x() + speed*diagonal, Global::Graphics_Manager()[graphics_key].y());
        }
    }
}

void Enemy::update(){
    if(is_on_screen()){
        if(!has_graphics) load_graphics(animation::Id::Walk);

        if(Global::Graphics_Manager()[Global::Player().get_graphics_key()].y() > sprite().y()){ sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() + 1);}
        else{ sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() - 1);}

        if(alive()) [[likely]] {
            _attack_update();

            // Movement and Animations
            if(get_state() == State::HURTING){
                if(Global::Graphics_Manager()[graphics_key].animation.done()){
                    set_state(State::NORMAL);
                    if(!_idle_time) _dir = NEUTRAL;
                    Global::Graphics_Manager()[graphics_key].configure_animation(id, _dir, animation::Id::Walk, position());
                }
            }else if(!is_attacking(40)){
                _movement();
            }
            // Combat
            if(Global::Player().alive()){
                bool player_attack_connected = Global::Player().get_state() == State::ATTACKING && Global::Player().get_hitbox().intersects(rect());
                if(player_attack_connected){
                    got_hit(Global::Player().get_attack());
                }
                bool attack_connected_player = get_state() == State::ATTACKING && is_in_range(Global::Player().position(), 25);
                if(attack_connected_player){
                    Global::Player().got_hit(Actor_data::stats[id].attack);
                }
            }

            _prev_dir = _dir;
        }
    }else{
        if(has_graphics){
            Global::Graphics_Manager().erase_sprite(graphics_key);
            has_graphics = false;
        }
    }
}


// ************* NPCs **************
NPC::~NPC() = default;


}