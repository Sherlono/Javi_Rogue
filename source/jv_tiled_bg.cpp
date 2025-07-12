#include "jv_tiled_bg.h"

#include "jv_math.h"
#include "jv_map_classes.h"

namespace jv{
tiled_bg::tiled_bg(game_map m, const uint8_t priority):
    map(m), prev_x(0), prev_y(0), bg_m_ptr(new bg_map()),
    bg(bn::regular_bg_item(bn::regular_bg_tiles_items::fortress_tiles, bn::bg_palette_items::fortress_palette, bg_m_ptr->map_item).create_bg(0, 0)),
    bg_m(bg.map())
    {
        bg.set_priority(priority);
    }

void tiled_bg::update(){
    int current_x = (Global::cam_pos().x() + 60)>>3  , current_y = (Global::cam_pos().y() + 44)>>3;
    int cx_pthtw = current_x + 32       , cy_pthtw = current_y + 32;

    if(current_x > prev_x){  // If moved Right
        for(int y = current_y; y < cy_pthtw; y++){
            uint8_t aux_x = current_x + 24;
            uint8_t ymod = bamod(y, 32);
            bool oob = aux_x - 16 >= map.x() || y - 15 <= 0 || y - 16 >= map.y();
            bn::point grid_coord(bamod(aux_x, 32), ymod);

            if(!oob){
                int cell_index = aux_x - 16 + (y - 16)*map.x();
                bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_m_ptr->set_cell(grid_coord, 0);
            }
        }
    }else if(current_x < prev_x){    // If moved Left
        for(int y = current_y; y < cy_pthtw; y++){
            uint8_t aux_x = current_x + 25;
            uint8_t ymod = bamod(y, 32);
            bool oob = aux_x - 47 <= 0 || y - 15 <= 0 || y - 16 >= map.y();
            bn::point grid_coord(bamod(aux_x, 32), ymod);

            if(!oob){
                int cell_index = aux_x - 48 + map.x() + (y - 17)*map.x();
                bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_m_ptr->set_cell(grid_coord, 0);
            }
        }
    }
    
    if(current_y > prev_y){    // If moved Down
        for(int x = current_x; x < cx_pthtw; x++){
            uint8_t aux_y = current_y + 24;
            bool oob = aux_y - 16 >= map.y() || x - 22 <= 0 || x - 23 >= map.x();
            bn::point grid_coord(bamod(x - 7, 32), bamod(current_y + 24, 32));

            if(!oob){
                int cell_index = x - 23 + (aux_y - 16)*map.x();
                bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_m_ptr->set_cell(grid_coord, 0);
            }
        }
    }else if(current_y < prev_y){   // If moved Up
        for(int x = current_x; x < cx_pthtw; x++){
            bool oob = current_y - 15 <= 0 || x - 22 <= 0 || x - 23 >= map.x();
            bn::point grid_coord(bamod(x - 7, 32), bamod(current_y, 32));

            if(!oob){
                int cell_index = x - 23 + (current_y - 16)*map.x();
                bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_m_ptr->set_cell(grid_coord, 0);
            }
        }
    }
    
    prev_x = current_x;
    prev_y = current_y;
    
    bg_m.reload_cells_ref();
}

void tiled_bg::init(){
    bg_m_ptr->reset();

    // Defining the MAP ARRAY bounds to redraw the map
    int current_x = Global::cam_pos().x()>>3    , current_y = (Global::cam_pos().y() + 43)>>3;
    int cx_pthtw = current_x + 32    , cy_pthtw = current_y + 32;
    // Redraw map bounds
    for(int y = current_y; y < cy_pthtw + 32; y++){
        for(int x = current_x; x < cx_pthtw; x++){
            if(x > cx_pthtw || y > cy_pthtw || x - 15 <= 0 || x - 16 >= map.x() || y - 15 <= 0 || y - 16 >= map.y()){ continue;}
            int xmod = bamod(x, 32), ymod = bamod(y, 32);

            bn::point grid_coord(xmod, ymod);

            int cell_index = x - 16 + (y - 16)*map.x();
            bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
        }
    }
    
    prev_x = current_x;
    prev_y = current_y;
    update();
}

}