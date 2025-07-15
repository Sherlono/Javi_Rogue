#ifndef JV_HEALTHBAR_H
#define JV_HEALTHBAR_H

#include "bn_array.h"
#include "bn_assert.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_healthbar_hud.h"
#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_healthbar_corner.h"

#define HB_Y_OFFSET -72

namespace jv{
struct healthbar{
    healthbar(short* m_v, short* v):
        max_value(m_v), value(v),
        _sprites( {bn::sprite_items::healthbar_hud.create_sprite(-104, HB_Y_OFFSET, 0), 
                   bn::sprite_items::healthbar_hud.create_sprite(-72, HB_Y_OFFSET, 1),
                   bn::sprite_items::healthbar.create_sprite(-99, HB_Y_OFFSET),
                   bn::sprite_items::healthbar_corner.create_sprite(-116, HB_Y_OFFSET)} )
        {
            for(bn::sprite_ptr sprite : _sprites){
                sprite.set_bg_priority(0);
            }
            _sprites[2].set_horizontal_scale(2.0);
        }

    // Getters
    bn::array<bn::sprite_ptr, 4>& sprites(){ return _sprites;}
    bool visible(){ return bar().visible();}

    // Setters
    void set_visible(bool visible){
        for(bn::sprite_ptr sprite : _sprites){
            sprite.set_visible(visible);
        }
    }

    void update(){
        BN_ASSERT(*max_value >= *value, "Hp can't surpass MaxHp");
        bn::fixed v(*value);
        bn::fixed m_v(*max_value);

        int target_x = (-147 + 48*(v/m_v)).integer();
        bar().set_x(target_x);
    }
    
private:
    // Getters
    bn::sprite_ptr& bar() { return _sprites[2];}

    short *max_value, *value;
    bn::array<bn::sprite_ptr, 4> _sprites;
};
}

#endif