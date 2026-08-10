#include "jv_projectile.h"

#include "jv_actors.h"
#include "jv_global.h"

namespace jv{
Projectile::Projectile(int x, int y):
    _anim_frames(0),
    _point(bn::point(x, y)),
    _moveVector(jv::normalize(bn::fixed_point(Global::Player().position().x(), Global::Player().position().y() + 8) - position())*_speed)
    {}

[[nodiscard]] bool Projectile::is_on_screen(uint8_t halfWidth, uint8_t halfHeight) const {
    uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = this->y() > Global::cam_pos().y() - y_offset, down = this->y() < Global::cam_pos().y() + y_offset;
    bool left = this->x() > Global::cam_pos().x() - x_offset, right = this->x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}

EnergyOrb::EnergyOrb(int x, int y):
    Projectile(x, y)
    {
        bn::sprite_builder builder(bn::sprite_items::energy_orb);
        builder.set_position(x, y);
        builder.set_camera(Global::Camera());
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build();
    }

bool EnergyOrb::update() {
    animation_update();
    set_position(x() + _moveVector.x(), y() + _moveVector.y());
    if(!is_on_screen(32, 32)){
        return true;
    }else if(Global::Player().alive()){
        if(Global::Player().rect().contains(_point)){
            Global::Player().got_hit(2);
            return true;
        }
    }
    return false;
}
}