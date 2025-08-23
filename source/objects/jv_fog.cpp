#include "jv_fog.h"

#include "jv_actors.h"
#include "jv_global.h"

namespace jv{
void iFog::update(){
    if(visible()){
        bool flag = false;
        for(int r = 0; r < this->size(); r++){
            if(_rooms[r].contains(Global::Player().position())){
                if(r != current_room){
                    set_shape(_rooms[r]);
                }
                current_room = r;
                flag = true;
                break;
            }
        }
        if(!flag){
            if(current_room != -1){
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
        for(int index = 0; index < _height; ++index){
            int qc_index = curve_line + 24 - _height;
            int x_m_camx = _x - Global::cam_pos().x();
            bn::fixed aux = (index > _height - 24)*(22*quarter_circle[qc_index]);
            bn::fixed w_m_aux = _width - aux;

            bn::pair<bn::fixed, bn::fixed> left_right(x_m_camx - w_m_aux, x_m_camx + w_m_aux);
            int upper_index = hdh_plus_y + index   ,   lower_index = hdh_plus_y - index - 1;
            if(upper_index >= 0 && upper_index < 160){ _horizontal_boundaries[upper_index] = left_right;}
            if(lower_index >= 0 && lower_index < 160){ _horizontal_boundaries[lower_index] = left_right;}
            curve_line += 1;
        }

        _horizontal_boundaries_hbe.reload_deltas_ref();
    }
}

}