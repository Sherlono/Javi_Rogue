#ifndef JV_GLOBAL_H
#define JV_GLOBAL_H

#include "bn_random.h"
#include "bn_assert.h"
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

using NPCs_vector_ptr_t = bn::ivector<jv::NPC>*;
using enemies_vector_ptr_t = bn::ivector<jv::Enemy*>*;
using items_vector_ptr_t = bn::ivector<jv::Item*>*;
using projectiles_vector_ptr_t = bn::ivector<jv::Projectile*>*;
using NPCs_vector_ref_t = bn::ivector<jv::NPC>&;
using enemies_vector_ref_t = bn::ivector<jv::Enemy*>&;
using items_vector_ref_t = bn::ivector<jv::Item*>&;
using projectiles_vector_ref_t = bn::ivector<jv::Projectile*>&;

struct Global{
public:
    Global() = delete;
    Global(const Global&) = delete;
    Global operator=(Global const& other) = delete;

    static void initialize(bn::camera_ptr* camera, GameMap* map, jv::Player* player, bn::random* randomizer, bn::ivector<jv::Projectile*>* projectiles);

    static void extra_data_init(bn::ivector<jv::NPC>* npcs, bn::ivector<jv::Enemy*>* enemies, bn::ivector<jv::Item*>* items);

    static void reset();

    static void update();

    static void create_projectile(int x, int y, uint8_t option);

    static void clear_enemies();
    static void clear_scene_items();
    static void clear_projectiles();
    static void clear_objects(){
        clear_enemies();
        clear_scene_items();
        clear_projectiles();
    }

    static void npcs_set_visible(bool visible);
    static void enemies_set_visible(bool visible);
    static void items_set_visible(bool visible);
    static void projectiles_set_visible(bool visible);

    static void scene_items_update();
    static bool enemy_obstacle(int x, int y, const uint8_t direction);
    static void enemies_update(bool& Objective);
    
    // Getters
    [[nodiscard]] static bn::camera_ptr& Camera();
    [[nodiscard]] static jv::Player& Player();
    [[nodiscard]] static GameMap& Map();
    [[nodiscard]] static bn::random& Random();
    [[nodiscard]] static NPCs_vector_ref_t NPCs();
    [[nodiscard]] static enemies_vector_ref_t Enemies();
    [[nodiscard]] static items_vector_ref_t Scene_Items();
    [[nodiscard]] static projectiles_vector_ref_t Projectiles();
    [[nodiscard]] static bn::point cam_pos();

private:
    static bn::camera_ptr* _camera;
    static GameMap* _map;
    static jv::Player* _player;
    static bn::random* _randomizer;
    inline static NPCs_vector_ptr_t _npcs;
    inline static enemies_vector_ptr_t _enemies;
    inline static items_vector_ptr_t _scene_items;
    inline static projectiles_vector_ptr_t _projectiles;
    static bn::point cam_position;
};

}

#endif