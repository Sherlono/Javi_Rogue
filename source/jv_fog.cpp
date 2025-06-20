#include "jv_fog.h"

namespace jv{
Fog::Fog():
    _x(0), _y(0), _width(0), _height(0), current_room(-1),
    _fog_bg(bn::regular_bg_items::darkness.create_bg(0, 0)),
    _window(bn::window::internal()),
    _internal_window(bn::rect_window::internal()),
    _horizontal_boundaries_hbe(bn::rect_window_boundaries_hbe_ptr::create_horizontal(_internal_window, _horizontal_boundaries))
    {
        _internal_window.set_top(_y - _height - Common::Camera().y());
        _internal_window.set_bottom(_y + _height - Common::Camera().y());
        _fog_bg.set_priority(0);
        _fog_bg.set_blending_enabled(true);
        _window.set_show_bg(_fog_bg, false);
    }

void Fog::update(){
    bn::point player_position = bn::point(Common::Player().position().x().integer(), Common::Player().position().y().integer());
    bool flag = false;
    for(int i = 0; i < _rooms.size(); i++){
        if(_rooms[i].contains(player_position)){
            if(i != current_room){
                reshape(_rooms[i]);
            }
            current_room = i;
            flag = true;
            break;
        }
    }
    if(!flag){
        reshape(player_position, 24, 24);
        current_room = -1;
    }
    
    int y_int = _y - Common::cam_pos().y();
    _internal_window.set_top(y_int - _height);
    _internal_window.set_bottom(y_int + _height);

    uint8_t curve_line = 0;
    for(int index = 0; index < _height; ++index){
        y_int = _y - Common::cam_pos().y();

        int qc_index = curve_line + 24 - _height;
        bn::fixed aux = (index > _height - 24)*(22*quarter_circle[qc_index]);
        bn::pair<bn::fixed, bn::fixed> left_right(_x - Common::Camera().x() - (_width - aux), _x - Common::Camera().x() + (_width - aux));
        int upper_index = (bn::display::height()>>1) + index + y_int;
        int lower_index = (bn::display::height()>>1) - index - 1 + y_int;
        if(upper_index >= 0 && upper_index < 160){ _horizontal_boundaries[upper_index] = left_right;}
        if(lower_index >= 0 && lower_index < 160){ _horizontal_boundaries[lower_index] = left_right;}
        curve_line += 1;
    }

    _horizontal_boundaries_hbe.reload_deltas_ref();
}

void Fog::create_room(bn::rect room){
    _rooms.push_back(room);
}

void Fog::set_boundaries(int w, int a){
    _internal_window.restore_boundaries();
    _width = w;
    _height = a;
    bn::fixed cam_y = Common::Camera().y();
    _internal_window.set_top(_y - _height - cam_y);
    _internal_window.set_bottom(_y + _height - cam_y);
}

}