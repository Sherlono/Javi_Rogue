#include "jv_fog.h"

#include "jv_actors.h"

namespace jv{
Fog::Fog(bn::camera_ptr cam):
    current_room(-1), _x(0), _y(0), _width(0), _height(0),
    _window(bn::window::internal()),
    _internal_window(bn::rect_window::internal()),
    _horizontal_boundaries_hbe(bn::rect_window_boundaries_hbe_ptr::create_horizontal(_internal_window, _horizontal_boundaries)),
    _fog_bg(bn::regular_bg_items::darkness.create_bg(0, 0))
    {
        _internal_window.set_top(_y - _height - cam.y());
        _internal_window.set_bottom(_y + _height - cam.y());
        _fog_bg.set_priority(0);
        _fog_bg.set_blending_enabled(true);
        _window.set_show_bg(_fog_bg, false);
    }

void Fog::update(){
    if(visible()){
        bool flag = false;
        for(int r = 0; r < _rooms.size(); r++){
            if(_rooms[r].contains(Global::Player().int_position())){
                if(r != current_room){
                    reshape(_rooms[r]);
                }
                current_room = r;
                flag = true;
                break;
            }
        }
        if(!flag){
            if(current_room != -1){
                reshape(Global::Player().int_position(), 24, 24);
                current_room = -1;
            }else{
                set_position(Global::Player().int_position());
            }
        }
        
        int y_int = _y - Global::cam_pos().y();
        _internal_window.set_top(y_int - _height);
        _internal_window.set_bottom(y_int + _height);

        int hdh_p_y = half_display_height + y_int;
        
        uint8_t curve_line = 0;
        for(int index = 0; index < _height; ++index){
            int qc_index = curve_line + 24 - _height;
            bn::fixed aux = (index > _height - 24)*(22*quarter_circle[qc_index]);
            bn::fixed w_minus_aux = _width - aux;

            bn::pair<bn::fixed, bn::fixed> left_right(_x - Global::Camera().x() - w_minus_aux, _x - Global::Camera().x() + w_minus_aux);
            int upper_index = hdh_p_y + index;
            int lower_index = hdh_p_y - index - 1;
            if(upper_index >= 0 && upper_index < 160){ _horizontal_boundaries[upper_index] = left_right;}
            if(lower_index >= 0 && lower_index < 160){ _horizontal_boundaries[lower_index] = left_right;}
            curve_line += 1;
        }

        _horizontal_boundaries_hbe.reload_deltas_ref();
    }
}

void Fog::create_room(bn::rect room){
    _rooms.push_back(room);
}

void Fog::set_boundaries(int w, int a){
    _width = w;
    _height = a;
    bn::fixed new_top = _y - _height - Global::Camera().y(), new_bottom = _y + _height - Global::Camera().y();
    if(new_top < _internal_window.bottom()){
        _internal_window.set_top(new_top);
        _internal_window.set_bottom(new_bottom);
    }else{
        _internal_window.set_bottom(new_bottom);
        _internal_window.set_top(new_top);
    }
}

}