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
struct itiled_bg{
    itiled_bg(): _bg_m_ptr(new bg_map()){}
    bn::unique_ptr<bg_map> _bg_m_ptr;
};


class tiled_bg: public itiled_bg, public bn::regular_bg_ptr{
public:
    tiled_bg(const bn::regular_bg_tiles_item& tiles_item, const bn::bg_palette_item& palette_item, int x, int y):
        bn::regular_bg_ptr(bn::regular_bg_item(tiles_item, palette_item, itiled_bg::_bg_m_ptr->map_item).create_bg(0, 0)),
        _map(x, y),
        _bg_m(bn::regular_bg_ptr::map())
        {}

    // Getters
    [[nodiscard]] int width() const { return _map.width();}
    [[nodiscard]] int height() const { return _map.height();}
    [[nodiscard]] int size() const { return _map.width()*_map.height();}
    [[nodiscard]] GameMap& map() { return _map;}
    [[nodiscard]] GameMap* map_ptr() { return &_map;}

    // Functionality
    void update();
    void init();

protected:
    GameMap _map;
    bn::regular_bg_map_ptr _bg_m;
    static int _prev_x, _prev_y;
};

}

#endif