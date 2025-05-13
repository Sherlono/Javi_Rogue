#ifndef JV_fog_bg_H
#define JV_fog_bg_H

#include "bn_rect.h"
//#include "bn_assert.h"
#include "bn_display.h"
#include "bn_blending.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_rect_window_actions.h"
#include "bn_rect_window_boundaries_hbe_ptr.h"

#include "jv_constants.h"

#include "bn_regular_bg_items_darkness.h"

namespace jv{

class Fog{
public:
    ~Fog(){ _rooms.clear();}
    Fog(bn::camera_ptr cam):
        _x(0), _y(0), _width(0), _height(0), current_room(-1),
        _fog_bg(bn::regular_bg_items::darkness.create_bg(0, 0)),
        _window(bn::window::internal()),
        _internal_window(bn::rect_window::internal()),
        _horizontal_boundaries_hbe(bn::rect_window_boundaries_hbe_ptr::create_horizontal(_internal_window, _horizontal_boundaries)),
        _cam(cam)
        {
            _internal_window.set_top(_y - _height - _cam.y());
            _internal_window.set_bottom(_y + _height - _cam.y());
            _fog_bg.set_priority(0);
            _fog_bg.set_blending_enabled(true);
            _window.set_show_bg(_fog_bg, false);
        }

    void update(bn::point cam_position){
        //bool flag = false;
        for(int i = 0; i < _rooms.size(); i++){
            if(i != current_room && _rooms[i].contains(cam_position)){
                reshape(_rooms[i]);
                current_room = i;
                //flag = true;
                break;
            }
        }
        /*if(!flag){
            reshape(cam_position, 24, 24);
            current_room = -1;
        }*/
        
        int y_int = _y - _cam.y().integer();
        _internal_window.set_top(y_int - _height);
        _internal_window.set_bottom(y_int + _height);

        uint8_t curve_line = 0;
        for(int index = 0; index < _height; ++index){
            y_int = _y - _cam.y().integer();

            int qc_index = curve_line + 24 - _height;
            bn::fixed aux = (index > _height - 24)*(22*quarter_circle[qc_index]);
            bn::pair<bn::fixed, bn::fixed> left_right(_x - _cam.x() - (_width - aux), _x - _cam.x() + (_width - aux));
            int upper_index = (bn::display::height()>>1) + index + y_int;
            int lower_index = (bn::display::height()>>1) - index - 1 + y_int;
            if(upper_index >= 0 && upper_index < 160){ _horizontal_boundaries[upper_index] = left_right;}
            if(lower_index >= 0 && lower_index < 160){ _horizontal_boundaries[lower_index] = left_right;}
            curve_line += 1;
        }

        _horizontal_boundaries_hbe.reload_deltas_ref();
    }

    inline void create_room(bn::rect room){
        _rooms.push_back(room);
    }

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

    void set_boundaries(int w, int a){
        _internal_window.restore_boundaries();
        _width = w;
        _height = a;
        bn::fixed cam_y = _cam.y();
        _internal_window.set_top(_y - _height - cam_y);
        _internal_window.set_bottom(_y + _height - cam_y);
    }

    void reshape(bn::point cam_position, int w, int a){
        set_position(cam_position.x(), cam_position.y());
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
    bn::camera_ptr _cam;
    bn::vector<bn::rect, 12> _rooms;
};

}

#endif