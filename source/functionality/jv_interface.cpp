#include "jv_interface.h"

#include "jv_map_classes.h"

namespace jv::Interface{
#if DEV_ENABLED
void Log_zone_layout([[maybe_unused]] GameMap& zone, bn::point player_cell){
    //BN_LOG("Level Layout. x: ", zone.width(), " y: ", zone.height());
    for(int y = 0; y < zone.height(); y++){
        bn::string_view line = "";
        for(int x = 0; x < zone.width(); x++){
            if(x == player_cell.x() && y == player_cell.y()) line = line + (bn::string<64>)("★");
            else line = line + (bn::string<64>)(zone.cell(x, y) ? "□ " : "■ ");
        }
        BN_LOG(line);
    }
}
#endif

}