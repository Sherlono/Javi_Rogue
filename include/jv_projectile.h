#ifndef JV_PROJECTILE_H
#define JV_PROJECTILE_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"
#include "bn_sprite_builder.h"

#include "jv_math.h"
#include "jv_common.h"

#include "bn_sprite_items_energy_orb.h"

namespace jv{
class Common;

class Projectile{
public:
    virtual ~Projectile() = default;
    Projectile(int x, int y);
    Projectile(const Projectile&) = delete;
    Projectile operator=(Projectile const& other) = delete;


    // Getters
    [[nodiscard]] bn::fixed x() const { return _sprite->x();}
    [[nodiscard]] bn::fixed y() const { return _sprite->y();}
    [[nodiscard]] bn::fixed_point position() const { return _point;}
    [[nodiscard]] inline bool on_screen(bn::camera_ptr& cam, uint8_t halfWidth = 16, uint8_t halfHeight = 16) const {
        uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
        bool up = this->y() > cam.y() - y_offset, down = this->y() < cam.y() + y_offset;
        bool left = this->x() > cam.x() - x_offset, right = this->x() < cam.x() + x_offset;
        return left && right && up && down;
    }

    // Setters
    void set_position(bn::fixed x, bn::fixed y){
        _point.set_x(x.integer());
        _point.set_y(y.integer());
        _sprite->set_position(x, y);
    };

    virtual void animation_update() = 0;

    virtual bool update() = 0;
    
protected:
    uint8_t _anim_frames;
    bn::point _point;
    bn::fixed_point _moveVector;
    bn::optional<bn::sprite_ptr> _sprite;
    
    static constexpr bn::fixed _speed = bn::fixed(1.8);
};
class EnergyOrb: public Projectile{
public:
    ~EnergyOrb(){ _sprite.reset();}
    EnergyOrb(int x, int y);

    void animation_update(){
        _anim_frames++;
        _sprite->set_tiles(bn::sprite_items::energy_orb.tiles_item().create_tiles(bamod(_anim_frames>>2, 4)));
    }

    bool update() override;
};

}

#endif