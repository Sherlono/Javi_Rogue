#ifndef JV_LEVELMAKER_H
#define JV_LEVELMAKER_H

#include "bn_camera_ptr.h"

#include "jv_math.h"
#include "jv_interface.h"
#include "jv_environment.h"

namespace jv::LevelMaker{
static int prev_x, prev_y;
void update(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map);

// Init must be called ONCE before the loop begins
void init(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    bg_map_ptr->reset();
    // Defining the MAP ARRAY bounds to redraw the map
    int current_x = (cam.x().integer())>>3  ,   current_y = (cam.y().integer() + 48)>>3;
    // Redraw map bounds
    for(int y = current_y; y < current_y + 32; y++){
        for(int x = current_x; x < current_x + 32; x++){
            int xmod = bamod(x, 32), ymod = bamod(y, 32);
            if(x > current_x + 34 || y > current_y + 34 || x - 15 <= 0 || x - 16 >= map.x() || y - 15 <= 0 || y - 16 >= map.y()){ continue;}

            bn::point grid_coord(xmod, ymod);

            int cell_index = x - 16 + (y - 16)*map.x();
            bg_map_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
        }
    }
    
    prev_x = current_x;
    prev_y = current_y;
    bg_map.reload_cells_ref();
    LevelMaker::update(cam, map, bg_map_ptr, bg_map);
}

// Update must be run every frame
void update(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    int current_x = (cam.x().integer() + 56)>>3  ,   current_y = (cam.y().integer() + 48)>>3;

    if(current_x > prev_x){  // If moved Right
        for(int y = current_y; y < 32 + current_y; y++){
            int aux_x = current_x + 24, ymod = bamod(y, 32);
            bool not_oob = (aux_x - 16 < map.x() && y - 15 > 0 && y - 16 < map.y());
            bn::point grid_coord(bamod(aux_x, 32), ymod);

            if(not_oob){
                int cell_index = aux_x - 16 + (y - 16)*map.x();
                bg_map_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_map_ptr->set_cell(grid_coord, 0);
            }
        }
    }else if(current_x < prev_x){    // If moved Left
        for(int y = current_y; y < 32 + current_y; y++){
            int aux_x = current_x + 25, ymod = bamod(y, 32);
            bool not_oob = (aux_x - 47 > 0 && y - 15 > 0 && y - 16 < map.y());
            bn::point grid_coord(bamod(aux_x, 32), ymod);

            if(not_oob){
                int cell_index = aux_x - 48 + map.x() + (y - 17)*map.x();
                bg_map_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_map_ptr->set_cell(grid_coord, 0);
            }
        }
    }
    
    if(current_y > prev_y){    // If moved Down
        for(int x = current_x; x < 32 + current_x; x++){
            int aux_y = current_y + 24;
            bool not_oob = (x - 22 > 0 && x - 23 < map.x() && aux_y - 16 < map.y());
            bn::point grid_coord(bamod(x - 7, 32), bamod(current_y + 24, 32));

            if(not_oob){
                int cell_index = x - 23 + (aux_y - 16)*map.x();
                bg_map_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_map_ptr->set_cell(grid_coord, 0);
            }
        }
    }else if(current_y < prev_y){   // If moved Up
        for(int x = current_x; x < 32 + current_x; x++){
            bool not_oob = (x - 22 > 0 && x - 23 < map.x() && current_y - 15 > 0);
            bn::point grid_coord(bamod(x - 7, 32), bamod(current_y, 32));

            if(not_oob){
                int cell_index = x - 23 + (current_y - 16)*map.x();
                bg_map_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
            }else{
                bg_map_ptr->set_cell(grid_coord, 0);
            }
        }
    }

    prev_x = current_x;
    prev_y = current_y;
    
    bg_map.reload_cells_ref();
}
}

#endif