#include "jv_common.h"

bn::camera_ptr* jv::Common::_camera = nullptr;
jv::Player* jv::Common::_player = nullptr;
game_map* jv::Common::_map = nullptr;
bn::random*jv::Common:: _randomizer = nullptr;
bn::point jv::Common::cam_position;

void jv::Common::initialize(bn::camera_ptr* camera, jv::Player* player, game_map* map, bn::random* randomizer, bn::ivector<jv::Projectile*>* projectiles){
    _camera = camera;
    _player = player;
    _map = map;
    _randomizer = randomizer;
    _projectiles = projectiles;
}

void jv::Common::reset(){
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

void jv::Common::update(){
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

void jv::Common::create_projectile(int x, int y){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    _projectiles->push_back(new EnergyOrb(x, y));
}

void jv::Common::clear_projectiles(){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    for(int i = 0; i < _projectiles->size(); i++){
        delete _projectiles->data()[i];
    }
    _projectiles->clear();
}

[[nodiscard]] bn::camera_ptr& jv::Common::Camera(){
    BN_ASSERT(_camera != nullptr, "Camera not found");
    return *_camera;
}
[[nodiscard]] jv::Player& jv::Common::Player(){
    BN_ASSERT(_player != nullptr, "Player not found");
    return *_player;
}
[[nodiscard]] game_map& jv::Common::Map(){
    BN_ASSERT(_map != nullptr, "Map not found");
    return *_map;
}
[[nodiscard]] bn::random& jv::Common::Random(){
    BN_ASSERT(_randomizer != nullptr, "Randomizer not found");
    return *_randomizer;
}
[[nodiscard]] bn::point jv::Common::cam_pos(){
    return cam_position;
}
