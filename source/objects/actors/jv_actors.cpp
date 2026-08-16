#include "jv_actors.h"

#include "bn_array.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_map_classes.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

namespace jv{
// ************** Actor *************
Actor::~Actor() { }

Actor::Actor(const uint8_t actor_id, const bn::point p):
    id(actor_id), graphics_key(255),
    _rect(bn::rect(p.x(), p.y(),
          Actor_data::meta[actor_id].rect_shape.width(),
          Actor_data::meta[actor_id].rect_shape.height())) { }
Actor::Actor(Actor& other){
    _prev_dir = other._prev_dir;
    _dir = other._dir;
    id = other.id;
    _rect = other._rect;
    graphics_key = 255;
}
void Actor::set_position(const bn::fixed x, const bn::fixed y){
    //BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Tried to position nonexistent sprite.");
    sprite().set_position(x, y);
    _rect.set_position(x.floor_integer(), y.floor_integer() + Actor_data::meta[id].sprt_y_offset);
}
void Actor::set_position(const bn::fixed_point point){
    //BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Tried to position nonexistent sprite.");
    sprite().set_position(point.x(), point.y());
    _rect.set_position(point.x().floor_integer(), point.y().floor_integer() + Actor_data::meta[id].sprt_y_offset);
}

bool Actor::Graphics::horizontal_flip = false;
animation::Id Actor::Graphics::action = animation::Id::Walk;
uint16_t Actor::Graphics::sprite_item_id = 0;
bn::fixed_point Actor::Graphics::sprite_position = bn::fixed_point(0, 0);
animation_type Actor::Graphics::frames = animation::W_down[0];

[[nodiscard]] bn::sprite_ptr& Actor::sprite() {
    //BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Tried to get nonexistent sprite.");
    return Global::Graphics_Manager()[graphics_key];
}
void Actor::set_visible(bool visible){
    //BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Tried to access nonexistent sprite.");
    sprite().set_visible(visible);
}
void Actor::set_camera(bn::camera_ptr& c){
    //BN_ASSERT(Global::Graphics_Manager().find(graphics_key), "Tried to access nonexistent sprite.");
    sprite().set_camera(c);
}

[[nodiscard]] bool Actor::is_on_screen() const {
    const int aux_x = _rect.x() - Global::cam_pos().x(), aux_y = _rect.y() - Global::cam_pos().y();
    if(aux_y <= -Actor_data::meta[id].on_screen_offset.height()){
        return false;
    }else if(aux_y >= Actor_data::meta[id].on_screen_offset.height()){
        return false;
    }else if(aux_x <= -Actor_data::meta[id].on_screen_offset.width()){
        return false;
    }else if(aux_x >= Actor_data::meta[id].on_screen_offset.width()){
        return false;
    }
    return true;
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
    Graphics::configure_animation(id, _dir, action, bn::point(x(), y() - Actor_data::meta[id].sprt_y_offset));
    Global::Graphics_Manager().create_sprite(graphics_key);

    /*bn::string_view actor_type_s = {};
    switch(id){
        case Actor_data::Id::Player:{
            actor_type_s = "Player";
            break;
        }
        case Actor_data::Id::Bad_Cat:{
            actor_type_s = "Bad_Cat";
            break;
        }
        case Actor_data::Id::Pale_Tongue:{
            actor_type_s = "Pale_Tongue";
            break;
        }
        case Actor_data::Id::Pale_Finger:{
            actor_type_s = "Pale_Finger";
            break;
        }
        case Actor_data::Id::Snakes:{
            actor_type_s = "Snakes";
            break;
        }
        case Actor_data::Id::Cow:{
            actor_type_s = "Cow";
            break;
        }
        case Actor_data::Id::Fox:{
            actor_type_s = "Fox";
            break;
        }
        default:{
            BN_BASIC_ASSERT("Bad id: ", id);
        }
    }

    BN_LOG("Sprite Created. Graphics key: ", graphics_key, " | Graphics Type: ", actor_type_s);*/
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

// Sets up an animation for actions with direction
void Actor::Graphics::configure_animation(const uint8_t actor_id, const uint8_t dir, const animation::Id act, const bn::fixed_point pos){
    sprite_item_id = actor_id;
    sprite_position = pos;
    action = act;

    switch(Actor::Graphics::action){
        case animation::Id::Walk:{
            if(dir == NORTH || dir == NORTHWEST || dir == NORTHEAST){        // UP
                horizontal_flip = false;
                frames = animation::W_up[Actor_data::meta[actor_id].anim_set];
            }else if(dir == SOUTH || dir == SOUTHWEST || dir == SOUTHEAST){  // DOWN
                horizontal_flip = false;
                frames = animation::W_down[Actor_data::meta[actor_id].anim_set];
            }else if(dir == WEST){                            // LEFT
                horizontal_flip = true;
                frames = animation::W_horiz[Actor_data::meta[actor_id].anim_set];
            }else if(dir == EAST){                            // RIGHT
                horizontal_flip = false;
                frames = animation::W_horiz[Actor_data::meta[actor_id].anim_set];
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
                frames = animation::Atk_up[Actor_data::meta[actor_id].anim_set];
            }else if(dir == SOUTH || dir == SOUTHWEST || dir == SOUTHEAST){  // DOWN
                horizontal_flip = false;
                frames = animation::Atk_do[Actor_data::meta[actor_id].anim_set];
            }else if(dir == WEST){                            // LEFT
                horizontal_flip = true;
                frames = animation::Atk_horiz[Actor_data::meta[actor_id].anim_set];
            }else if(dir == EAST){                            // RIGHT
                horizontal_flip = false;
                frames = animation::Atk_horiz[Actor_data::meta[actor_id].anim_set];
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
            frames = animation::W_down[Actor_data::meta[actor_id].anim_set];
            break;
        }
    }
}

// Releases previously configured directed action animation
bn::sprite_animate_action<animation::MAX_FRAMES> Actor::Graphics::create_animation(){
    set_horizontal_flip(horizontal_flip);
    if(action != animation::Id::Attack){
        return bn::sprite_animate_action<animation::MAX_FRAMES>::forever(*this,
                                                                         Actor_data::data[sprite_item_id].wait_frames,
                                                                         Actor_data::data[sprite_item_id].sprite_item_ref.tiles_item(),
                                                                         frames);
    }
    else {
        return bn::sprite_animate_action<animation::MAX_FRAMES>::once(*this,
                                                                      4,
                                                                      Actor_data::data[sprite_item_id].sprite_item_ref.tiles_item(),
                                                                      frames);
    }
}


// ************** Enemy *************
Enemy::~Enemy() { }

void Enemy::got_hit(int attack){
    Actor::Graphics& my_graphics = Global::Graphics_Manager()[graphics_key];
    const uint8_t dmg = attack/Actor_data::stats[id].defense;

    _attack_cooldown = 0;
    _prev_attack_cooldown = 0;
    hp = dmg > hp ? 0 : hp - dmg;
    if(hp > 0) [[likely]] {
        _state = State::HURTING;
        my_graphics.set_horizontal_flip(_dir == WEST);
        my_graphics.animation = bn::sprite_animate_action<animation::MAX_FRAMES>::once(my_graphics, 8, Actor_data::data[id].sprite_item_ref.tiles_item(), animation::hurt[Actor_data::meta[id].anim_set]);
    }else{
        my_graphics.set_horizontal_flip(false);
        _state = State::DEAD;
    }
}

void Enemy::_start_attack(Graphics& my_graphics){
    bool is_finger = id == Actor_data::Pale_Finger;
    if(!(_attack_cooldown + is_finger*_idle_time)){
        _attack_cooldown = Actor_data::meta[id].atk_end_cooldown;
        Actor::Graphics::configure_animation(id, _dir, animation::Id::Attack, position());
        my_graphics.animation = my_graphics.create_animation();
        if(is_finger) Global::create_projectile(x(), y() - 40, Projectile::IDs::ENERGYORB);
    }
}
void Enemy::_attack_update(Graphics& my_graphics){
    _prev_attack_cooldown = _attack_cooldown;
    if(_attack_cooldown) _attack_cooldown--;
    if(_state != State::HURTING){
        _state = State::NORMAL;
        if(_attack_cooldown == 40) _state = State::ATTACKING;
    }
    if(attack_ended(40)){
        look_at(Global::Player().normalized_vector(position()));

        Actor::Graphics::configure_animation(id, _dir, animation::Id::Walk, position());
        my_graphics.animation = my_graphics.create_animation();
    }
}

void Enemy::_simple_fsm_update(Graphics& my_graphics){    
    // Movement and Animations
    if(get_state() == State::HURTING){
        if(my_graphics.animation.done()){
            set_state(State::NORMAL);
            if(!_idle_time){
                _dir = NEUTRAL;
            }
            Actor::Graphics::configure_animation(id, _dir, animation::Id::Walk, position());
            my_graphics.animation = my_graphics.create_animation();
        }
    }else if(!is_attacking(40)){
        _movement(my_graphics);
    }

    // Combat
    Player& player_ref = Global::Player();
    if(player_ref.alive()){
        bool player_attack_connected = player_ref.get_state() == State::ATTACKING && player_ref.get_hitbox().intersects(rect());
        if(player_attack_connected){
            got_hit(player_ref.get_attack());
        }
        if(id != Actor_data::Pale_Finger) {
            bool attack_connected_player = get_state() == State::ATTACKING && is_in_range(player_ref.position(), 25);
            if(attack_connected_player){
                player_ref.got_hit(Actor_data::stats[id].attack);
            }
        }
    }
}

// Random direction
void Enemy::_idle(Graphics& my_graphics){
    if(_idle_time == 0){
        _dir = Global::Random().get_int(12);
        _idle_time++;
    }else if(_idle_time <= 1*60 + _dir*2){
        _idle_time++;
    }else{
        _idle_time = 0;
    }

    _displace(bn::fixed(0.5), my_graphics.position());
}

void Enemy::_movement(Graphics& my_graphics){
    bn::fixed_point player_direction = Global::Player().normalized_vector(position() - bn::point(0, 8*(id == Actor_data::Pale_Finger)));
    
    // Player within range
    if(!Global::Player().invisible){
        if(is_in_range(Global::Player().position(), Actor_data::meta[id].attack_range)){
            look_at(player_direction);
            if(id != Actor_data::Pale_Finger){
                _start_attack(my_graphics);
                
                if(_idle_time <= 2*60){
                    _idle_time++;
                }else{
                    _idle_time = 0;
                }
            }
            else{
                if(_idle_time <= 2*60){
                    _idle_time++;
                }
                bn::fixed target_x = my_graphics.x(), target_y = my_graphics.y();

                if((player_direction.x() < 0 && _map_obstacle(EAST)) || (player_direction.x() > 0 && _map_obstacle(WEST))){
                    target_x -= player_direction.x()*Actor_data::stats[id].speed;
                }
                if((player_direction.y() < 0 && _map_obstacle(SOUTH)) || (player_direction.y() > 0 && _map_obstacle(NORTH))){
                    target_y -= player_direction.y()*Actor_data::stats[id].speed;
                }

                set_position(target_x, target_y);
            }
        }else if(is_in_range(Global::Player().position(), Actor_data::meta[id].view_range)){
            look_at(player_direction);
            
            if(id != Actor_data::Pale_Finger){
                if(_idle_time <= 2*60) _idle_time++;
                
                bn::fixed target_x = my_graphics.x(), target_y = my_graphics.y();
                if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
                    target_x += player_direction.x()*Actor_data::stats[id].speed;
                }
                if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
                    target_y += player_direction.y()*Actor_data::stats[id].speed;
                }
                
                set_position(target_x, target_y);
            }
            else{
                if(_idle_time == 0){
                    _start_attack(my_graphics);
                    _idle_time++;
                }else if(_idle_time <= 2*60){
                    _idle_time++;
                }else{
                    _idle_time = 0;
                }
            }
        }
        else{
            _idle(my_graphics);
        }
        
    }
    else{
        _idle(my_graphics);
    }
    
    if(_state == State::NORMAL && !is_attacking(40)){
        if(_prev_dir != _dir){
            Actor::Graphics::configure_animation(id, _dir, animation::Id::Walk, position());
            my_graphics.animation = my_graphics.create_animation();
            _rotate_rect();
        }
    }
}

void Enemy::_displace(const bn::fixed speed, bn::fixed_point position){
    // If direction is valid
    if(_dir != NEUTRAL && _dir < 9){
        bn::fixed x_offset = 0, y_offset = 0;
        // Move if dir not obstructed
        if((_dir == NORTH || _dir == NORTHWEST || _dir == NORTHEAST) && _map_obstacle(NORTH)){          // UP
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == NORTHEAST);
            y_offset = -speed*diagonal; 
        }else if((_dir == SOUTH || _dir == SOUTHWEST || _dir == SOUTHEAST) && _map_obstacle(SOUTH)){  // DOWN
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == SOUTHWEST || _dir == SOUTHEAST);
            y_offset = speed*diagonal; 
        }
        if((_dir == WEST || _dir == NORTHWEST || _dir == SOUTHWEST) && _map_obstacle(WEST)){  // LEFT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHWEST || _dir == SOUTHWEST);
            x_offset = - speed*diagonal; 
        }else if((_dir == EAST || _dir == NORTHEAST || _dir == SOUTHEAST) && _map_obstacle(EAST)){ // RIGHT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(_dir == NORTHEAST || _dir == SOUTHEAST);
            x_offset = speed*diagonal; 
        }
        set_position(position.x() + x_offset, position.y() + y_offset); 
    }
}

void Enemy::update(){
    if(is_on_screen()){
        if(!Global::Graphics_Manager().find(graphics_key)) load_graphics(animation::Id::Walk);
        
        Graphics& my_graphics = Global::Graphics_Manager()[graphics_key];
        bn::sprite_ptr& player_sprite = Global::Player().sprite();

        if(Global::Player().y() > y()){ my_graphics.set_z_order(player_sprite.z_order() + 1); }
        else{ my_graphics.set_z_order(player_sprite.z_order() - 1); }

        if(alive()) [[likely]] {
            _attack_update(my_graphics);
            _simple_fsm_update(my_graphics);
        }

        _prev_dir = _dir;
    }else{
        if(Global::Graphics_Manager().find(graphics_key)) Global::Graphics_Manager().erase_sprite(graphics_key);
    }
}


// ************* NPCs **************
NPC::~NPC(){ }

}