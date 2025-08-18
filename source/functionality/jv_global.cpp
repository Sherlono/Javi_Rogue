#include "jv_global.h"

#include "bn_point.h"

#include "jv_items.h"
#include "jv_actors.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

namespace jv{
bn::camera_ptr* Global::_camera = nullptr;
GameMap* Global::_map = nullptr;
Player* Global::_player = nullptr;
bn::random* Global::_randomizer = nullptr;
bn::point Global::cam_position;

void Global::initialize(bn::camera_ptr* camera, GameMap* map, jv::Player* player, bn::random* randomizer, projectiles_vector_ptr_t projectiles){
    _camera = camera;
    _player = player;
    _map = map;
    _randomizer = randomizer;
    _projectiles = projectiles;
}

void Global::reset(){
    _camera = nullptr;
    _player = nullptr;
    _map = nullptr;
    _projectiles = nullptr;
}

void Global::update(){
    if(_camera != nullptr){
        cam_position = bn::point(_camera->position().x().integer(), _camera->position().y().integer());
    }
}

void Global::create_projectile(int x, int y, uint8_t option){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    switch(option){
        case 0:{
            _projectiles->push_back(new EnergyOrb(x, y));
            break;
        }
        default:
            BN_ASSERT(false, "Invalid Projectile id");
            break;
    }
}

[[nodiscard]] bn::camera_ptr& Global::Camera(){
    BN_ASSERT(_camera != nullptr, "Camera not found");
    return *_camera;
}
[[nodiscard]] Player& Global::Player(){
    BN_ASSERT(_player != nullptr, "Player not found");
    return *_player;
}
[[nodiscard]] GameMap& Global::Map(){
    BN_ASSERT(_map != nullptr, "Map not found");
    return *_map;
}
[[nodiscard]] bn::random& Global::Random(){
    BN_ASSERT(_randomizer != nullptr, "Randomizer not found");
    return *_randomizer;
}
[[nodiscard]] projectiles_vector_ref_t Global::Projectiles(){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    return *_projectiles;
}

[[nodiscard]] bn::point Global::cam_pos(){
    return cam_position;
}

}