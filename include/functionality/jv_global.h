#ifndef JV_GLOBAL_H
#define JV_GLOBAL_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"

#include "jv_constants.h"

class GameMap;

namespace jv{
class Item;
class Player;
class Enemy;
class NPC;
class Projectile;

using projectiles_vector_ptr_t = bn::ivector<jv::Projectile*>*;
using projectiles_vector_ref_t = bn::ivector<jv::Projectile*>&;

struct Global{
public:
    Global() = delete;
    Global(const Global&) = delete;
    Global operator=(Global const& other) = delete;

    static void initialize(bn::camera_ptr* camera, GameMap* map, jv::Player* player, bn::random* randomizer, bn::ivector<jv::Projectile*>* projectiles);

    static void reset();

    static void update();

    static void create_projectile(int x, int y, uint8_t option);

    // Getters
    [[nodiscard]] static bn::camera_ptr& Camera();
    [[nodiscard]] static jv::Player& Player();
    [[nodiscard]] static GameMap& Map();
    [[nodiscard]] static bn::random& Random();
    [[nodiscard]] static projectiles_vector_ref_t Projectiles();
    [[nodiscard]] static bn::point cam_pos();

private:
    static bn::camera_ptr* _camera;
    static GameMap* _map;
    static jv::Player* _player;
    static bn::random* _randomizer;
    inline static projectiles_vector_ptr_t _projectiles;
    static bn::point cam_position;
};

}

#endif