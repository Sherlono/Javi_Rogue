#ifndef JV_HEALTHBAR_H
#define JV_HEALTHBAR_H

#include "bn_array.h"
#include "bn_assert.h"
#include "bn_sprite_ptr.h"

#include "jv_global.h"

namespace jv{
class Player;

struct healthbar{
    healthbar();
    
    // Getters
    bn::array<bn::sprite_ptr, 4>& sprites(){ return _sprites;}
    bool visible(){ return bar().visible();}

    // Setters
    void set_visible(bool visible){
        for(bn::sprite_ptr sprite : _sprites){
            sprite.set_visible(visible);
        }
    }

    // Functionality
    void init();

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