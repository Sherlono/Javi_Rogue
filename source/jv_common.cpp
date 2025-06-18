#include "jv_common.h"

namespace jv{
bn::camera_ptr* Common::_camera = nullptr;
Player* Common::_player = nullptr;
game_map* Common::_map = nullptr;
bn::random* Common::_randomizer = nullptr;
bn::point Common::cam_position;

void Common::initialize(bn::camera_ptr* camera, jv::Player* player, game_map* map, bn::random* randomizer, bn::ivector<Projectile*>* projectiles){
    _camera = camera;
    _player = player;
    _map = map;
    _randomizer = randomizer;
    _projectiles = projectiles;
}

void Common::reset(){
    _camera = nullptr;
    _player = nullptr;
    _map = nullptr;
    _randomizer = nullptr;
    for(int i = 0; i < _projectiles->size(); i++){
        delete _projectiles->data()[i];
    }
    _projectiles->clear();
    _projectiles = nullptr;
}

void Common::update(){
    if(_camera != nullptr){
        cam_position = bn::point(_camera->position().x().integer(), _camera->position().y().integer());
    }

    if(_projectiles != nullptr){
        for(int i = 0; i < _projectiles->size(); i++){
            if(_projectiles->data()[i]->update()){
                delete _projectiles->data()[i];
                _projectiles->erase(_projectiles->begin() + i);
            }
        }
    }
}

void Common::create_projectile(int x, int y){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    _projectiles->push_back(new EnergyOrb(x, y));
}

void Common::clear_projectiles(){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    for(int i = 0; i < _projectiles->size(); i++){
        delete _projectiles->data()[i];
    }
    _projectiles->clear();
}

[[nodiscard]] bn::camera_ptr& Common::Camera(){
    BN_ASSERT(_camera != nullptr, "Camera not found");
    return *_camera;
}
[[nodiscard]] Player& Common::Player(){
    BN_ASSERT(_player != nullptr, "Player not found");
    return *_player;
}
[[nodiscard]] game_map& Common::Map(){
    BN_ASSERT(_map != nullptr, "Map not found");
    return *_map;
}
[[nodiscard]] bn::random& Common::Random(){
    BN_ASSERT(_randomizer != nullptr, "Randomizer not found");
    return *_randomizer;
}
[[nodiscard]] bn::point Common::cam_pos(){
    return cam_position;
}

}