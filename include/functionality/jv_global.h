#ifndef JV_GLOBAL_H
#define JV_GLOBAL_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"

#include "jv_fog.h"
#include "jv_constants.h"

class GameMap;

namespace jv{
class tiled_bg;
class NPC;
class Item;
class Enemy;
class Player;
class Stairs;
class Projectile;
struct GameAssets;

using NPCs_ref_t = bn::ivector<NPC>&;
using enemy_ref_t = bn::ivector<Enemy*>&;
using projectiles_ref_t = bn::ivector<Projectile*>&;

struct Global{
public:
    enum Environments {Fortress, Jungle};
    Global() = delete;
    Global(const Global&) = delete;
    Global operator=(Global const& other) = delete;

    static void initialize(bn::camera_ptr* cam, jv::tiled_bg* t_bg, GameAssets* assets = nullptr);
    static void reset(){
        _cam = nullptr;
        _tiled_bg = nullptr;
        _assets = nullptr;
        environment_id = 0;
    }
    static void update();
    static void create_projectile(const int x, const  int y, const uint8_t option);

    // Getters
    [[nodiscard]] static bn::camera_ptr& Camera();
    [[nodiscard]] static jv::Player& Player();
    [[nodiscard]] static jv::Stairs& Stairs();
    [[nodiscard]] static jv::Fog<MAX_ROOMS>& Fog();
    [[nodiscard]] static jv::tiled_bg& Tiled_Bg();
    [[nodiscard]] static bn::random& Random();
    [[nodiscard]] static NPCs_ref_t NPCs();
    [[nodiscard]] static enemy_ref_t Enemies();
    [[nodiscard]] static projectiles_ref_t Projectiles();
    [[nodiscard]] static bn::point cam_pos();

    static uint8_t environment_id;
private:
    static bn::camera_ptr* _cam;
    static jv::tiled_bg* _tiled_bg;
    static GameAssets* _assets;
    static bn::point cam_position;
};

}

#endif