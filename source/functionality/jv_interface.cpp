#include "jv_interface.h"

#include "jv_map_classes.h"

namespace jv::Interface{
void Log_zone_layout([[maybe_unused]] GameMap& zone){
#if DEV_ENABLED
    BN_LOG("Level Layout");
    for(int y = 0; y < zone.height(); y++){
        bn::string_view line = "";
        for(int x = 0; x < zone.width(); x++){
            line = line + (bn::string<64>)(zone.cell(x, y) ? "□ " : "■ ");
        }
        BN_LOG(line);
    }
#endif
}

}