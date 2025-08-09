#include "jv_healthbar.h"
#include "jv_actors.h"

#include "bn_sprite_items_healthbar_hud.h"
#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_healthbar_corner.h"

#define HB_Y_OFFSET -72

namespace jv{
class Player;
healthbar::healthbar():
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

void healthbar::init(){
    max_value = Global::Player().get_maxhp_ptr();
    value = Global::Player().get_hp_ptr();
}
}