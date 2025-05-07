#ifndef JV_HEALTHBAR
#define JV_HEALTHBAR

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_healthbar_corner.h"

namespace jv{
struct healthbar{
    healthbar(short* m_v, short* v):
        max_value(m_v), value(v)
        {
            _sprites.push_back(bn::sprite_items::healthbar.create_sprite(-84, -72));
            _sprites.push_back(bn::sprite_items::healthbar_corner.create_sprite(-117, -72));
            _sprites[0].set_bg_priority(0);
            _sprites[0].set_horizontal_scale(2.0);
            _sprites[0].set_blending_enabled(true);
            _sprites[1].set_bg_priority(0);
            _sprites[1].set_blending_enabled(true);
        }

    // Getters
    bn::sprite_ptr bar_sprite(){ return _sprites[0];}
    bn::sprite_ptr corner_sprite(){ return _sprites[1];}

    void update(){
        bn::fixed v(*value);
        bn::fixed m_v(*max_value);
        int target_x = (-148 + 64*(v/m_v)).integer();
        _sprites[0].set_x(target_x);
    }
private:
    short *max_value, *value;
    bn::vector<bn::sprite_ptr, 2> _sprites;
};
}

#endif