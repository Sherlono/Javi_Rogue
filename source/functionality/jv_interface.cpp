#include "jv_interface.h"

#include "jv_actors.h"
#include "jv_healthbar.h"
#include "jv_tiled_bg.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

namespace jv::Interface{
void Log_zone_layout([[maybe_unused]] Zone& zone){
    #if DEV_ENABLED
        BN_LOG("Level Layout");
        for(int y = 0; y < zone._height; y++){
            bn::string_view line = "";
            for(int x = 0; x < zone._width; x++){
                line = line + (bn::string<32>)(zone.cell(x, y) ? "□ " : "■ ");
            }
            BN_LOG(line);
        }
    #endif
}

void set_hide_all(jv::healthbar& healthbar, bn::regular_bg_ptr& background, jv::tiled_bg& Fortress, bn::ivector<bn::sprite_ptr>& txt, bool hide){
    healthbar.set_visible(!hide);
    background.set_visible(!hide);
    Fortress.set_visible(!hide);
    jv::Global::Player().set_visible(!hide);
    jv::Global::npcs_set_visible(!hide);
    jv::Global::enemies_set_visible(!hide);
    jv::Global::items_set_visible(!hide);
    jv::Global::projectiles_set_visible(!hide);
    for(bn::sprite_ptr sprite : txt){ sprite.set_visible(!hide);}
}

}