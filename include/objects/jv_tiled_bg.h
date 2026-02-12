#ifndef JV_TILED_BG_H
#define JV_TILED_BG_H

#include "bn_point.h"
#include "bn_unique_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_tiles_ptr.h"

#include "jv_global.h"
#include "jv_map_classes.h"

class GameMap;

namespace jv{
class tiled_bg{
public:
    tiled_bg(const bn::regular_bg_tiles_item& tiles_item, const bn::bg_palette_item& palette_item, GameMap& m):
        map(m), _bg_m_ptr(new bg_map()),
        _bg(bn::regular_bg_item(tiles_item, palette_item, _bg_m_ptr->map_item).create_bg(0, 0)),
        _bg_m(_bg.map())
        {}

    // Getters
    [[nodiscard]] int width() const { return map.width();}
    [[nodiscard]] int height() const { return map.height();}
    [[nodiscard]] int size() const { return map.width()*map.height();}
    [[nodiscard]] bn::fixed_point position() const { return _bg.position();}
    [[nodiscard]] bn::regular_bg_ptr& background() { return _bg;}
    [[nodiscard]] bn::regular_bg_tiles_ptr tiles() { return _bg.tiles();}
    [[nodiscard]] bool visible() { return _bg.visible();}

    // Setters
    void set_camera(bn::camera_ptr& camera){
        _bg.set_camera(camera);
    }
    void remove_camera(){
        _bg.remove_camera();
    }
    void set_priority(int priority){
        _bg.set_priority(priority);
    }
    void set_visible(bool visible){ _bg.set_visible(visible);}

    void set_tiles(const bn::regular_bg_tiles_ptr& tiles){
        _bg.set_tiles(tiles);
    }
    void set_tiles(const bn::regular_bg_tiles_item& tiles_item){
        _bg.set_tiles(tiles_item);
    }
    void set_tiles(bn::regular_bg_tiles_ptr&& tiles){
        _bg.set_tiles(tiles);
    }
    void set_palette(const bn::bg_palette_ptr& palette){
        _bg.set_palette(palette);
    }
    void set_palette(const bn::bg_palette_item& palette_item){
        _bg.set_palette(palette_item);
    }


    // Functionality
    void update();
    void init();

    GameMap& map;
protected:
    bn::unique_ptr<bg_map> _bg_m_ptr;
    bn::regular_bg_ptr _bg;
    bn::regular_bg_map_ptr _bg_m;
    static int _prev_x, _prev_y;
};

}

#endif