#ifndef JV_HEALTHBAR
#define JV_HEALTHBAR

#include "bn_vector.h"
#include "bn_assert.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_healthbar_hud.h"
#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_healthbar_corner.h"

namespace jv{
struct healthbar{
    healthbar(short* m_v, short* v):
        max_value(m_v), value(v)
        {
            _sprites.push_back(bn::sprite_items::healthbar_hud.create_sprite(-104, -72, 0));
            _sprites.push_back(bn::sprite_items::healthbar_hud.create_sprite(-72, -72, 1));
            _sprites.push_back(bn::sprite_items::healthbar.create_sprite(-99, -72));
            _sprites.push_back(bn::sprite_items::healthbar_corner.create_sprite(-116, -72));
            _sprites[0].set_bg_priority(0);
            _sprites[1].set_bg_priority(0);
            _sprites[2].set_bg_priority(0);
            _sprites[2].set_horizontal_scale(2.0);
            _sprites[3].set_bg_priority(0);
        }

    // Getters
    bn::vector<bn::sprite_ptr, 4>& sprites(){ return _sprites;}
    bool visible(){ return bar().visible();}

    // Setters
    void set_visible(bool visible){
        _sprites[0].set_visible(visible);
        _sprites[1].set_visible(visible);
        _sprites[2].set_visible(visible);
        _sprites[3].set_visible(visible);
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
    bn::vector<bn::sprite_ptr, 4> _sprites;
};
}

#endif