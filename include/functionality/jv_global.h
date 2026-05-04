#ifndef JV_GLOBAL_H
#define JV_GLOBAL_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"

#include "jv_fog.h"
#include "jv_constants.h"

namespace jv{
class tiled_bg;
class NPC;
class Item;
class Enemy;
class Player;
class Stairs;
class Projectile;
class GameMap;
struct GameAssets;

using NPCs_ref_t = bn::ivector<NPC*>&;
using enemy_ref_t = bn::ivector<Enemy*>&;
using projectiles_ref_t = bn::ivector<Projectile*>&;

struct Global{
public:
    Global() = delete;
    Global(const Global&) = delete;
    Global operator=(Global const& other) = delete;

    static uint8_t environment_id;
    enum Environments {Fortress, Jungle};

    // Functionality
    static void init(bn::camera_ptr* cam, jv::tiled_bg* t_bg, GameAssets* assets = nullptr);
    static void reset();
    static void update();   // Must be called at the start of any game loop
    static void create_projectile(const int x, const  int y, const uint8_t option);
    static void update_entity_animations();
    static void clear_bg_map();
    
    // Getters
    [[nodiscard]] static bn::camera_ptr& Camera();
    [[nodiscard]] static jv::Player& Player();
    [[nodiscard]] static jv::Stairs& Stairs();
    [[nodiscard]] static jv::Fog<MAX_ROOMS>& Fog();
    [[nodiscard]] static jv::tiled_bg& Tiled_Bg();
    [[nodiscard]] static jv::GameMap& Map();
    [[nodiscard]] static bn::random& Random();
    [[nodiscard]] static NPCs_ref_t NPCs();
    [[nodiscard]] static enemy_ref_t Enemies();
    [[nodiscard]] static projectiles_ref_t Projectiles();
    [[nodiscard]] static bn::point cam_pos(){
        return _cam_position;
    }
    [[nodiscard]] static bn::point prev_cam_pos(){
        return _prev_cam_pos;
    }
    [[nodiscard]] static bool cam_moved(){
        return _cam_moved;
    }

    static bool autoCamControl;
private:
    static constexpr bn::fixed _cam_lerp_value = bn::fixed(0.16);
    
    static bool _cam_moved;
    static bn::point _cam_position, _prev_cam_pos;
    static bn::fixed_point _cam_target;

    static bn::camera_ptr* _cam;
    static jv::GameAssets* _assets;
    static jv::tiled_bg* _tiled_bg;
};

}

#endif