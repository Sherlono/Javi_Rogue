#include "jv_tiled_bg.h"

#include "jv_math.h"
#include "jv_map_classes.h"

namespace jv{
int tiled_bg::prev_x = 0, tiled_bg::prev_y = 0;

void tiled_bg::init(){
    bg_m_ptr->reset();

    const int current_x = Global::cam_pos().x()>>3    , current_y = (Global::cam_pos().y() + 43)>>3;
    const int cx_plus_32 = current_x + 32    , cy_plus_32 = current_y + 32;
    uint8_t value;
    bool flip;
    
    for(int y = current_y; y < cy_plus_32; y++){
        for(int x = current_x; x < cx_plus_32; x++){
            if(x > cx_plus_32 || y > cy_plus_32 || x - 15 <= 0 || x - 16 >= map.x() || y - 15 <= 0 || y - 16 >= map.y()){ continue;}
            const int xmod = bamod(x, 32), ymod = bamod(y, 32);
            const int cell_index = x - 16 + (y - 16)*map.x();

            value = map[cell_index];
            flip = map.horizontal_flip(cell_index);
            
            bg_m_ptr->set_cell(xmod, ymod, value, flip);
        }
    }
    
    prev_x = current_x;
    prev_y = current_y;
    update();
}

void tiled_bg::update(){
    const int current_x = (Global::cam_pos().x() + 60)>>3   ,   current_y = (Global::cam_pos().y() + 44)>>3;
    const int cx_plus_32 = current_x + 32   ,   cy_plus_32 = current_y + 32;
    uint8_t value;
    bool flip;

    bn::bg_tiles::set_allow_offset(false);

    if(current_x > prev_x){  // If moved Right
        const short aux_x = current_x + 24, x_minus_16 = aux_x - 16;
        for(int y = current_y; y < cy_plus_32; y++){
            const short ymod = bamod(y, 32), y_minus_16 = y - 16;
            const bool oob = x_minus_16 >= map.x() || y_minus_16 < 0 || y_minus_16 >= map.y();

            const int cell_index = x_minus_16 + y_minus_16*map.x();
            if(!oob){
                value = map[cell_index];
                flip = map.horizontal_flip(cell_index);
            }else{
                value = 0;
                flip = false;
            }
            bg_m_ptr->set_cell(bamod(aux_x, 32), ymod, value, flip);
        }
    }else if(current_x < prev_x){    // If moved Left
        const short aux_x = current_x + 25, x_minus_48 = aux_x - 48;
        for(int y = current_y; y < cy_plus_32; y++){
            const short ymod = bamod(y, 32), y_minus_16 = y - 16;
            const bool oob = x_minus_48 < 0 || y_minus_16 < 0 || y_minus_16 >= map.y();

            int cell_index = x_minus_48 + map.x() + (y - 17)*map.x();
            if(!oob){
                value = map[cell_index];
                flip = map.horizontal_flip(cell_index);
            }else{
                value = 0;
                flip = false;
            }
            bg_m_ptr->set_cell(bamod(aux_x, 32), ymod, value, flip);
        }
    }
    
    if(current_y > prev_y){    // If moved Down
        const short aux_y = current_y + 24, y_minus_16 = aux_y - 16;
        for(int x = current_x; x < cx_plus_32; x++){
            const short x_minus_23 = x - 23;
            const bool oob = y_minus_16 >= map.y() || x_minus_23 < 0 || x_minus_23 >= map.x();

            const int cell_index = x_minus_23 + y_minus_16*map.x();
            if(!oob){
                value = map[cell_index];
                flip = map.horizontal_flip(cell_index);
            }else{
                value = 0;
                flip = false;
            }
            bg_m_ptr->set_cell(bamod(x - 7, 32), bamod(current_y + 24, 32), value, flip);
        }
    }else if(current_y < prev_y){   // If moved Up
        const short y_minus_16 = current_y - 16;
        for(int x = current_x; x < cx_plus_32; x++){
            const short x_minus_23 = x - 23;
            const bool oob = y_minus_16 < 0 || x_minus_23 < 0 || x_minus_23 >= map.x();

            const int cell_index = x_minus_23 + y_minus_16*map.x();
            if(!oob){
                value = map[cell_index];
                flip = map.horizontal_flip(cell_index);
            }else{
                value = 0;
                flip = false;
            }
            bg_m_ptr->set_cell(bamod(x - 7, 32), bamod(current_y, 32), value, flip);
        }
    }
    bn::bg_tiles::set_allow_offset(true);
    
    prev_x = current_x;
    prev_y = current_y;
    
    bg_m.reload_cells_ref();
}

}