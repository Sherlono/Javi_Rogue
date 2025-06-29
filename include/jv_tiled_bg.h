#ifndef JV_TILED_BG_H
#define JV_TILED_BG_H

#include "bn_unique_ptr.h"
#include "bn_camera_ptr.h"

#include "jv_common.h"
#include "jv_map_classes.h"

#include "bn_regular_bg_tiles_items_floor_tiles.h"
#include "bn_bg_palette_items_floor_palette.h"

class game_map;

namespace jv{
class tiled_bg{
public:
    tiled_bg(game_map m, const uint8_t priority = 0);
    
    // Getters
    [[nodiscard]] int width() const { return map.x();}
    [[nodiscard]] int height() const { return map.y();}
    [[nodiscard]] bn::fixed_point position() const { return bg.position();}

    // Setters
    void set_camera(bn::camera_ptr& camera){
        bg.set_camera(camera);
    }
    void remove_camera(){
        bg.remove_camera();
    }
    void set_priority(int priority){
        bg.set_priority(priority);
    }
    void set_visible(bool visible){ bg.set_visible(visible);}
    void set_blending_enabled(bool isBlend){ bg.set_blending_enabled(isBlend);}

    // Functionality
    void update();
    void init();

    game_map map;
private:
    int prev_x, prev_y;
    bn::unique_ptr<bg_map> bg_m_ptr;
    bn::regular_bg_ptr bg;
    bn::regular_bg_map_ptr bg_m;
};

}

#endif