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
bn::camera_ptr* Global::_cam = nullptr;
jv::tiled_bg* Global::_tiled_bg = nullptr;
GameAssets* Global::_assets = nullptr;
bn::point Global::cam_position;
uint8_t Global::environment_id = Environments::Fortress;


void Global::initialize(bn::camera_ptr* cam, jv::tiled_bg* t_bg, GameAssets* assets){
    _cam = cam;
    _tiled_bg = t_bg;
    _assets = assets;
    environment_id = 0;
}

void Global::update(){
    if(_assets != nullptr){
        cam_position = bn::point(_cam->position().x().floor_integer(), _cam->position().y().floor_integer());
    }
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

[[nodiscard]] bn::point Global::cam_pos(){
    return cam_position;
}

}