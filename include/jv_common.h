#ifndef JV_COMMON_H
#define JV_COMMON_H

#include "bn_assert.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_random.h"

#include "jv_actors.h"
#include "jv_constants.h"
#include "jv_projectile.h"
#include "jv_environment.h"

class game_map;

namespace jv{
class Player;
class Projectile;
class EnergyOrb;

struct Common{
public:
    Common(const Common&) = delete;
    Common operator=(Common const& other) = delete;

    static void initialize(bn::camera_ptr* camera = nullptr, jv::Player* player = nullptr, game_map* map = nullptr, bn::random* randomizer = nullptr, bn::ivector<jv::Projectile*>* projectiles = nullptr);

    static void reset();

    static void update();

    static void create_projectile(int x, int y);

    static void clear_projectiles();

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
    inline static bn::ivector<jv::Projectile*>* _projectiles;
    static bn::point cam_position;
};

}

#endif