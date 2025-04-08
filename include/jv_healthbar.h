#ifndef JV_HEALTHBAR
#define JV_HEALTHBAR

#include "bn_sprite_ptr.h"

#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_healthbar_corner.h"

namespace jv{
class healthbar{
public:
    healthbar(int* m_v, int* v):
        max_value(m_v), value(v),
        bar(bn::sprite_items::healthbar.create_sprite(-84, -72)),
        corner(bn::sprite_items::healthbar_corner.create_sprite(-117, -72))
        {
            bar.set_bg_priority(0);
            bar.set_horizontal_scale(2.0);
            corner.set_bg_priority(0);
            corner.set_z_order(bar.z_order() - 1);
        }
    void update(){
        bn::fixed v(*value);
        bn::fixed m_v(*max_value);
        int target_x = (-148 + 64*(v/m_v)).integer();
        bar.set_x(target_x);
    }

private:
    int *max_value, *value;
    bn::sprite_ptr bar, corner;
};
}

#endif