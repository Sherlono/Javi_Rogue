#ifndef JV_fog_bg_H
#define JV_fog_bg_H

#include "bn_rect.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_blending.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_rect_window_actions.h"
#include "bn_rect_window_boundaries_hbe_ptr.h"

#include "jv_constants.h"
#include "jv_global.h"

#include "bn_regular_bg_items_darkness.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

namespace jv{
class Player;

class Fog{
public:
    ~Fog(){ _rooms.clear();}
    Fog(bn::camera_ptr cam);

    // Getters
    bool visible(){ return _fog_bg.visible();}

    void update();

    void create_room(bn::rect room);
    
    inline void reset(){
        current_room = -1;
        _rooms.clear();
    }

private:
    // Setters
    void set_position(int x, int y){
        _x = x;
        _y = y;
    }

    void set_visible(bool visible){
        _fog_bg.set_visible(visible);
    }

    void set_boundaries(int w, int a);

    void reshape(bn::point player_position, int w, int a){
        set_position(player_position.x(), player_position.y());
        set_boundaries(w, a);
    }
    
    void reshape(bn::rect shape){
        set_position(shape.x(), shape.y());
        set_boundaries(shape.width()>>1, shape.height()>>1);
    }

    int _x, _y;
    int _width, _height;
    int current_room;
    bn::regular_bg_ptr _fog_bg;
    bn::window _window;
    bn::rect_window _internal_window;
    bn::array<bn::pair<bn::fixed, bn::fixed>, bn::display::height()> _horizontal_boundaries;
    bn::rect_window_boundaries_hbe_ptr _horizontal_boundaries_hbe;
    bn::vector<bn::rect, 16> _rooms;
};

}

#endif