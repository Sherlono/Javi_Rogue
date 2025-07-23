#include "jv_global.h"

#include "jv_items.h"
#include "jv_actors.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

namespace jv{
bn::camera_ptr* Global::_camera = nullptr;
game_map* Global::_map = nullptr;
Player* Global::_player = nullptr;
bn::random* Global::_randomizer = nullptr;
bn::point Global::cam_position;

void Global::initialize(bn::camera_ptr* camera, game_map* map, jv::Player* player, bn::random* randomizer, bn::ivector<Projectile*>* projectiles){
    _camera = camera;
    _player = player;
    _map = map;
    _randomizer = randomizer;
    _projectiles = projectiles;
}

void Global::extra_data_init(bn::ivector<jv::NPC>* npcs, bn::ivector<jv::Enemy*>* enemies, bn::ivector<jv::Item*>* items){
    _npcs = npcs;
    _enemies = enemies;
    _scene_items = items;
}

void Global::reset(){
    _camera = nullptr;
    _player = nullptr;
    _map = nullptr;
    _randomizer = nullptr;
    _npcs = nullptr;
    _enemies = nullptr;
    _scene_items = nullptr;
    for(int i = 0; i < _projectiles->size(); i++){
        delete _projectiles->data()[i];
    }
    _projectiles->clear();
    _projectiles = nullptr;
}

void Global::update(){
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

void Global::clear_enemies(){
    BN_ASSERT(_enemies != nullptr, "Enemies not found");
    for(int i = 0; i < _enemies->size(); i++){
        delete _enemies->data()[i];
    }
    _enemies->clear();
}

void Global::clear_scene_items(){
    BN_ASSERT(_scene_items != nullptr, "Items not found");
    for(int i = 0; i < _scene_items->size(); i++){
        delete _scene_items->data()[i];
    }
    _scene_items->clear();
}

void Global::clear_projectiles(){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    for(int i = 0; i < _projectiles->size(); i++){
        delete _projectiles->data()[i];
    }
    _projectiles->clear();
}

void Global::npcs_set_visible(bool visible){
    BN_ASSERT(_npcs != nullptr, "NPCs not found");
    for(int i = 0; i < _npcs->size(); i++){ _npcs->data()[i].set_visible(visible);}
}
void Global::enemies_set_visible(bool visible){
    BN_ASSERT(_enemies != nullptr, "Enemies not found");
    for(int i = 0; i < _enemies->size(); i++){ _enemies->data()[i]->set_visible(visible);}
}
void Global::items_set_visible(bool visible){
    BN_ASSERT(_scene_items != nullptr, "Items not found");
    for(int i = 0; i < _scene_items->size(); i++){ _scene_items->data()[i]->set_visible(visible);}
}
void Global::projectiles_set_visible(bool visible){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    for(int i = 0; i < _projectiles->size(); i++){ _projectiles->data()[i]->set_visible(visible);}
}

void Global::scene_items_update(){
    for(int i = 0; i < _scene_items->size(); i++){
        if(!_scene_items->data()[i]->gotten()){
            _scene_items->data()[i]->update();
        }else{
            delete _scene_items->data()[i];
            _scene_items->erase(_scene_items->begin() + i);
        }
    }
}

bool Global::enemy_obstacle(int x, int y, const uint8_t direction){
    switch(direction){
        case Actor::Direction::NORTH:{
            bn::point p(x, y - 8);
            for(int i = 0; i < _enemies->size(); i++){
                if(_enemies->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::SOUTH:{
            bn::point p(x, y + 8);
            for(int i = 0; i < _enemies->size(); i++){
                if(_enemies->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::WEST:{
            bn::point p(x - 8, y);
            for(int i = 0; i < _enemies->size(); i++){
                if(_enemies->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        case Actor::Direction::EAST:{
            bn::point p(x + 8, y);
            for(int i = 0; i < _enemies->size(); i++){
                if(_enemies->data()[i]->rect().contains(p)){
                    return false;
                }
            }
            break;
        }
        default:
            BN_ASSERT(false, "Invalid direction", direction);
            break;
    }
    return true;
}

void Global::enemies_update(bool& Objective){
    for(int i = 0; i < _enemies->size(); i++){
        _enemies->data()[i]->update();
        Objective = Objective && !_enemies->data()[i]->alive();
        if(_enemies->data()[i]->get_state() == Actor::State::DEAD){
            int item_check = _randomizer->get_int(0, 3);
            if(item_check == 1){
                _scene_items->push_back(new jv::Potion(_enemies->data()[i]->int_x(), _enemies->data()[i]->int_y()));
            }else if(item_check == 2){
                _scene_items->push_back(new jv::Key(_enemies->data()[i]->int_x(), _enemies->data()[i]->int_y()));
            }
            delete _enemies->data()[i];
            _enemies->erase(_enemies->begin() + i);
        }
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
[[nodiscard]] game_map& Global::Map(){
    BN_ASSERT(_map != nullptr, "Map not found");
    return *_map;
}
[[nodiscard]] bn::random& Global::Random(){
    BN_ASSERT(_randomizer != nullptr, "Randomizer not found");
    return *_randomizer;
}
[[nodiscard]] bn::ivector<jv::NPC>& Global::NPCs(){
    BN_ASSERT(_npcs != nullptr, "NPCs not found");
    return *_npcs;
}
[[nodiscard]] bn::ivector<jv::Enemy*>& Global::Enemies(){
    BN_ASSERT(_enemies != nullptr, "Enemies not found");
    return *_enemies;
}
[[nodiscard]] bn::ivector<jv::Item*>& Global::Scene_Items(){
    BN_ASSERT(_scene_items != nullptr, "Scene Items not found");
    return *_scene_items;
}
[[nodiscard]] bn::ivector<jv::Projectile*>& Global::Projectiles(){
    BN_ASSERT(_projectiles != nullptr, "Projectiles not found");
    return *_projectiles;
}

[[nodiscard]] bn::point Global::cam_pos(){
    return cam_position;
}

}