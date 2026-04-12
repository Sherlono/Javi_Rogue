#include "jv_fog.h"

#include "jv_actors.h"
#include "jv_global.h"

namespace jv{
void iFog::update(){
    bool flag = false;
    if(_size != 0) {
        for(int r = 0; r < this->size(); r++){
            if(_shapes[r].contains(Global::Player().position())){
                if(r != current_room){
                    set_shape(_shapes[r]);
                }
                current_room = r;
                flag = true;
                break;
            }
        }
    }

    if(_size == 0 || !flag){  // If player is not inside a Fog room
        if(current_room != -1) [[unlikely]] {
            set_shape(Global::Player().position(), 24, 24);
            current_room = -1;
        }else{
            set_position(Global::Player().position());
        }
    }
    
    const int y_int = _y - Global::cam_pos().y();
    _internal_window.set_top(y_int - _height);
    _internal_window.set_bottom(y_int + _height);

    const int hdh_plus_y = half_display_height + y_int;
    
    uint8_t curve_line = 0;
    const int x_int = Global::cam_pos().x();
    for(int index = 0; index < _height; ++index){
        const int qc_index = curve_line + 24 - _height;
        const int x_m_camx = _x - x_int;
        bn::fixed aux;
        if(index > _height - 24){ aux = _width - (22*quarter_circle[qc_index]).floor_integer(); }
        else aux = _width;

        const bn::pair<int, int> left_right(x_m_camx - aux, x_m_camx + aux);
        const int upper_index = hdh_plus_y + index   ,   lower_index = hdh_plus_y - index - 1;
        if(upper_index >= 0 && upper_index < 160){ _horizontal_boundaries[upper_index] = left_right;}
        if(lower_index >= 0 && lower_index < 160){ _horizontal_boundaries[lower_index] = left_right;}
        curve_line += 1;
    }

    _horizontal_boundaries_hbe.reload_deltas_ref();
}

}