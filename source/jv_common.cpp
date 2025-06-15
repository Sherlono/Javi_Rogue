#include "jv_common.h"

bn::camera_ptr* jv::Common::_camera;
jv::Player* jv::Common::_player;
game_map* jv::Common::_map;
bn::random*jv::Common:: _randomizer;
bn::vector<jv::Projectile*, MAX_ENEMIES> jv::Common::_projectiles;
bn::point jv::Common::cam_position;
jv::Common* jv::Common::instance;

void jv::Common::initialize(bn::camera_ptr* camera, jv::Player* player, game_map* map, bn::random* randomizer){
    _camera = camera;
    _player = player;
    _map = map;
    _randomizer = randomizer;
    //_projectiles = projectiles;
    BN_ASSERT(_camera != NULL && _player != NULL && _randomizer != NULL, "Common failed to initialize");
}

void jv::Common::reset(){
    _camera = NULL;
    _player = NULL;
    _map = NULL;
    _randomizer = NULL;
    //_projectiles = NULL;
    for(int i = 0; i < _projectiles.size(); i++){
        delete _projectiles[i];
    }
    _projectiles.clear();
}

void jv::Common::update(){
    cam_position = bn::point(_camera->position().x().integer(), _camera->position().y().integer());

    for(int i = 0; i < _projectiles.size(); i++){
        if(_projectiles[i]->update()){
            delete _projectiles[i];
            _projectiles.erase(_projectiles.begin() + i);
        }
    }
}

void jv::Common::create_projectile(int x, int y){
    _projectiles.push_back(new EnergyOrb(x, y));
}
void jv::Common::clear_projectiles(){
    for(int i = 0; i < _projectiles.size(); i++){
        delete _projectiles[i];
    }
    _projectiles.clear();
}

[[nodiscard]] bn::camera_ptr& jv::Common::Camera(){ return *_camera;}
[[nodiscard]] jv::Player& jv::Common::Player(){ return *_player;}
[[nodiscard]] game_map& jv::Common::Map(){ return *_map;}
[[nodiscard]] bn::random& jv::Common::Random(){ return *_randomizer;}
[[nodiscard]] bn::point jv::Common::cam_pos(){ return cam_position;}
