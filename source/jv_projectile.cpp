#include "jv_projectile.h"

#include "jv_actors.h"

namespace jv{
Projectile::Projectile(int x, int y):
    _anim_frames(0),
    _point(bn::point(x, y)),
    _moveVector(jv::normalize(bn::fixed_point(Global::Player().position().x(), Global::Player().position().y() + 8) - position())*_speed)
    {}

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
    if(!on_screen(Global::Camera(), 32, 32)){
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