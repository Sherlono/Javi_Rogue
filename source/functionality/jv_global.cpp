#include "jv_global.h"

#include "bn_point.h"

#include "jv_fog.h"
#include "jv_items.h"
#include "jv_stairs.h"
#include "jv_actors.h"
#include "jv_tiled_bg.h"
#include "jv_projectile.h"
#include "jv_game_assets.h"
#include "jv_map_classes.h"

namespace jv{
bool Global::_cam_moved = false;
bool Global::autoCamControl = true;
uint8_t Global::environment_id = Environments::Fortress;
bn::point Global::_cam_position = {0, 0}, Global::_prev_cam_pos = {0, 0};
bn::fixed_point Global::_cam_target;

bn::camera_ptr* Global::_cam = nullptr;
jv::GameAssets* Global::_assets = nullptr;
jv::tiled_bg* Global::_tiled_bg = nullptr;

void Global::init(bn::camera_ptr* cam, jv::tiled_bg* t_bg, GameAssets* assets){
    _cam = cam;
    _tiled_bg = t_bg;
    _assets = assets;
    environment_id = 0;
}

void Global::reset(){
    if(_assets != nullptr){
        _assets->clear_objects();
        _assets->fog.clear();
        _assets = nullptr;
    }
    _cam = nullptr;
    _tiled_bg = nullptr;

    environment_id = 0;
    autoCamControl = true;
    _cam_moved = false;
    _cam_position = {0, 0};
    _prev_cam_pos = {0, 0};
    _cam_target = {0, 0};
    
    bn::core::update();
}

void Global::update(){
    if(_cam != nullptr){
        _prev_cam_pos = _cam_position;

        if(autoCamControl){
            _cam_target = lerp(Camera().position(), Player().get_hitbox().position() + bn::point(0, 4), _cam_lerp_value);
            _cam->set_position(_cam_target);
        }
        _cam_position = bn::point(_cam->position().x().floor_integer(), _cam->position().y().floor_integer());
        
        _cam_moved = _prev_cam_pos != _cam_position;
    }
    if(_cam_moved) Global::Tiled_Bg().update();
    /* Other possible code */
}

void Global::create_projectile(const int x,const  int y,const  uint8_t option){
    switch(option){
        case 0:{
            _assets->v_projectiles.push_back(new EnergyOrb(x, y));
            break;
        }
        default:
            BN_ERROR("Invalid Projectile id: ", option);
            break;
    }
}

void Global::update_entity_animations(){
    Global::update();
    if(!_assets->cat.graphics.animation->done()) _assets->cat.graphics.animation->update();
    for(auto npc : _assets->v_npcs) if(npc->on_screen() && !npc->graphics.animation->done()) npc->graphics.animation->update();
}

void Global::clear_bg_map(){
    _tiled_bg->game_map().clear();
}

[[nodiscard]] bn::camera_ptr& Global::Camera(){
    return *_cam;
}
[[nodiscard]] jv::Player& Global::Player(){
    return _assets->cat;
}
[[nodiscard]] jv::Stairs& Global::Stairs(){
    return _assets->stairs;
}
[[nodiscard]] jv::Fog<MAX_ROOMS>& Global::Fog(){
    return _assets->fog;
}
[[nodiscard]] jv::tiled_bg& Global::Tiled_Bg(){
    return *_tiled_bg;
}
[[nodiscard]] jv::GameMap& Global::Map(){
    return *_tiled_bg->game_map_ptr();
}
[[nodiscard]] bn::random& Global::Random(){
    return _assets->randomizer;
}

[[nodiscard]] NPCs_ref_t Global::NPCs(){
    return _assets->v_npcs;
}
[[nodiscard]] enemy_ref_t Global::Enemies(){
    return _assets->v_enemies;
}
[[nodiscard]] projectiles_ref_t Global::Projectiles(){
    return _assets->v_projectiles;
}

}