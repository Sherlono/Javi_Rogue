#include "jv_projectile.h"

namespace jv{
Projectile::Projectile(int x, int y):
    _anim_frames(0),
    _point(bn::point(x, y)),
    _moveVector(jv::normalize(bn::fixed_point(Common::Player().position().x(), Common::Player().position().y() + 8) - position())*_speed)
    {}

EnergyOrb::EnergyOrb(int x, int y):
    Projectile(x, y)
    {
        bn::sprite_builder builder(bn::sprite_items::energy_orb);
        builder.set_position(x, y);
        builder.set_camera(Common::Camera());
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build();
    }

bool EnergyOrb::update() {
    animation_update();
    set_position(x() + _moveVector.x(), y() + _moveVector.y());
    if(!on_screen(Common::Camera(), 32, 32)){
        return true;
    }else if(Common::Player().alive()){
        if(Common::Player().rect().contains(_point)){
            Common::Player().got_hit(2);
            return true;
        }
    }
    return false;
}
}