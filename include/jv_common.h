#ifndef JV_COMMON_H
#define JV_COMMON_H

#include "bn_random.h"
#include "bn_assert.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"

#include "jv_constants.h"

class game_map;

namespace jv{
class Item;
class Player;
class Enemy;
class NPC;
class Projectile;

struct Common{
public:
    Common(const Common&) = delete;
    Common operator=(Common const& other) = delete;

    static void initialize(bn::camera_ptr* camera = nullptr, jv::Player* player = nullptr, game_map* map = nullptr, bn::random* randomizer = nullptr, bn::ivector<jv::Projectile*>* projectiles = nullptr);

    static void extra_data_init(bn::ivector<jv::NPC>* npcs = nullptr, bn::ivector<jv::Enemy*>* enemies = nullptr, bn::ivector<jv::Item*>* items = nullptr);

    static void reset();

    static void update();

    static void create_projectile(int x, int y, uint8_t option);

    static void clear_enemies();
    static void clear_scene_items();
    static void clear_projectiles();

    static void npcs_set_visible(bool visible);
    static void enemies_set_visible(bool visible);
    static void items_set_visible(bool visible);
    static void projectiles_set_visible(bool visible);

    // Getters
    [[nodiscard]] static bn::camera_ptr& Camera();
    [[nodiscard]] static jv::Player& Player();
    [[nodiscard]] static game_map& Map();
    [[nodiscard]] static bn::random& Random();
    [[nodiscard]] static bn::point cam_pos();

private:
    static bn::camera_ptr* _camera;
    static jv::Player* _player;
    static game_map* _map;
    static bn::random* _randomizer;
    inline static bn::ivector<jv::NPC>* _npcs;
    inline static bn::ivector<jv::Enemy*>* _enemies;
    inline static bn::ivector<jv::Item*>* _scene_items;
    inline static bn::ivector<jv::Projectile*>* _projectiles;
    static bn::point cam_position;
};

}

#endif