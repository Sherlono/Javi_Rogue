#include "jv_projectile.h"

jv::Projectile::Projectile(int x, int y):
    _point(bn::point(x, y)),
    _moveVector(jv::normalize(bn::fixed_point(jv::Common::Player().position().x(), jv::Common::Player().position().y() + 8) - position())*_speed)
    {}

jv::EnergyOrb::EnergyOrb(int x, int y):
    Projectile(x, y)
    {
        bn::sprite_builder builder(bn::sprite_items::energy_orb);
        builder.set_position(x, y);
        builder.set_camera(jv::Common::Camera());
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build();
    }

bool jv::EnergyOrb::update(bool isInvul) {
    set_position(x() + _moveVector.x(), y() + _moveVector.y());
    if(!on_screen(jv::Common::Camera(), 8, 8)){
        return true;
    }else if(jv::Common::Player().alive()){
        if(!isInvul && jv::Common::Player().rect().contains(_point)){
            jv::Common::Player().got_hit(2);
            return true;
        }
    }
    return false;
}