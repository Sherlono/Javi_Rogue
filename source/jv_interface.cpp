#include "jv_interface.h"

#include "jv_actors.h"
#include "jv_healthbar.h"
#include "jv_tiled_bg.h"
#include "jv_stairs.h"
#include "jv_projectile.h"
#include "jv_map_classes.h"

namespace jv::Interface{
void Log_zone_layout([[maybe_unused]] Zone& zone){
    #if LOGS_ENABLED
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

void set_hide_all(jv::healthbar& healthbar, jv::stairs& stairs, bn::regular_bg_ptr& background, jv::tiled_bg& Fortress, bn::ivector<bn::sprite_ptr>& txt, bool hide){
    stairs.set_visible(!hide);
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

void random_coords(bn::ivector<bn::point>& points_out){
    bn::point* valid_points = nullptr;
    int pointCount = 0;

    {
        int current_size = 0;
        int width = (Global::Map().x() - 2)>>2, height = (Global::Map().y() - 3)>>2;
        int tileIndex[4] = {0, 0, 0, 0};

        // Finding coordinates with floor in them
        for(int y = 2; y < height; y++){
            for(int x = 1; x < width; x++){
                bool w_check = true; // Walkable check
                tileIndex[0] = x*4 - 1 + (y*4 - 1)*Global::Map().x();
                tileIndex[1] = x*4 + 2 + (y*4 - 1)*Global::Map().x();
                tileIndex[2] = x*4 - 1 + (y*4 + 2)*Global::Map().x();
                tileIndex[3] = x*4 + 2 + (y*4 + 2)*Global::Map().x();

                for(int i = 0; i < 4; i++){
                    w_check = w_check && (Global::Map()[tileIndex[i]] > 0 && Global::Map()[tileIndex[i]] < WTILES_COUNT);
                }
                
                if(w_check){
                    // Simple dinamically growing array code. Double size when capped
                    pointCount++;
                    if(valid_points == nullptr){
                        valid_points = new bn::point[1];
                        valid_points[0] = bn::point(32*x, 32*y);
                        current_size = 1;
                    }else if(pointCount >= current_size){
                        current_size = current_size*2;
                        bn::point* temp = new bn::point[current_size];
                        for(int i = 0; i < pointCount - 1; i++){
                            temp[i] = valid_points[i];
                        }
                        temp[pointCount - 1] = bn::point(32*x, 32*y);

                        delete[] valid_points;
                        valid_points = temp;
                    }else{
                        valid_points[pointCount - 1] = bn::point(32*x, 32*y);
                    }
                }
            }
        }
    }
    // Storing random valid coordinates
    for(int i = 0; i < points_out.max_size(); i++){
        int pointIndex = Global::Random().get_int(0, pointCount);
        points_out.push_back(pop_point(valid_points, pointCount, pointIndex));
    }
    delete[] valid_points;
}

}