#ifndef JV_TILED_BG_H
#define JV_TILED_BG_H

#include "bn_point.h"
#include "bn_array.h"
#include "bn_limits.h"
#include "bn_unique_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_tiles_ptr.h"

#include "jv_global.h"
#include "jv_map_classes.h"

#include "bn_bg_palette_items_fortress_palette.h"
#include "bn_regular_bg_tiles_items_fortress_tiles1.h"
#include "bn_regular_bg_tiles_items_fortress_tiles2.h"
#include "bn_regular_bg_tiles_items_fortress_tiles3.h"

class game_map;

namespace jv{
class tiled_bg{
public:
    tiled_bg(const bn::regular_bg_tiles_item& tiles_item, const bn::bg_palette_item& palette_item, game_map m):
        map(m), bg_m_ptr(new bg_map()),
        bg(bn::regular_bg_item(tiles_item, palette_item, bg_m_ptr->map_item).create_bg(0, 0)),
        bg_m(bg.map())
        {}

    // Getters
    [[nodiscard]] int width() const { return map.x();}
    [[nodiscard]] int height() const { return map.y();}
    [[nodiscard]] int size() const { return map.x()*map.y();}
    [[nodiscard]] bn::fixed_point position() const { return bg.position();}
    [[nodiscard]] bn::regular_bg_ptr& background() { return bg;}
    [[nodiscard]] bool visible() { return bg.visible();}

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

    // Functionality
    void update();
    void init();

    game_map map;
protected:
    bn::unique_ptr<bg_map> bg_m_ptr;
    bn::regular_bg_ptr bg;
    bn::regular_bg_map_ptr bg_m;
    static int prev_x, prev_y;
};

}

#endif