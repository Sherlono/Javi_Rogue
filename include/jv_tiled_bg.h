#ifndef JV_TILED_BG_H
#define JV_TILED_BG_H

#include "bn_unique_ptr.h"
#include "bn_camera_ptr.h"

#include "jv_math.h"
#include "jv_interface.h"

#include "bn_regular_bg_tiles_items_floor_tiles.h"
#include "bn_bg_palette_items_floor_palette.h"

class game_map;

namespace jv{
class tiled_bg{
public:
    tiled_bg(game_map m, const uint8_t priority = 0):
        map(m), prev_x(0), prev_y(0), bg_m_ptr(new bg_map()),
        bg(bn::regular_bg_item(bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_m_ptr->map_item).create_bg(0, 0)),
        bg_m(bg.map())
        {
            bg.set_priority(priority);
        }

    // Getters
    [[nodiscard]] int width() const { return map.x();}
    [[nodiscard]] int height() const { return map.y();}
    [[nodiscard]] bn::fixed x() const { return bg.x();}
    [[nodiscard]] bn::fixed y() const { return bg.y();}
    [[nodiscard]] bn::fixed_point position() const { return bg.position();}

    // Setters
    void set_camera(bn::camera_ptr& camera){
        bg.set_camera(camera);
    }
    void remove_camera(){
        bg.remove_camera();
    }
    void set_priority(int priority){
        bg.set_priority(priority);
    }
    void set_visible(bool visible){ bg.set_visible(visible);}
    void set_blending_enabled(bool isBlend){ bg.set_blending_enabled(isBlend);}

    // Update must be run every frame
    void update(bn::camera_ptr const& cam){
        int intcam_x = cam.x().integer()    , intcam_y = cam.y().integer();
        int current_x = (intcam_x + 56)>>3  , current_y = (intcam_y + 48)>>3;
        int cx_pthtw = current_x + 32       , cy_pthtw = current_y + 32;

        if(current_x > prev_x){  // If moved Right
            for(int y = current_y; y < cy_pthtw; y++){
                uint8_t aux_x = current_x + 24;
                uint8_t ymod = bamod(y, 32);
                bool not_oob = (aux_x - 16 < map.x() && y - 15 > 0 && y - 16 < map.y());
                bn::point grid_coord(bamod(aux_x, 32), ymod);

                if(not_oob){
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
                bool not_oob = (aux_x - 47 > 0 && y - 15 > 0 && y - 16 < map.y());
                bn::point grid_coord(bamod(aux_x, 32), ymod);

                if(not_oob){
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
                bool not_oob = (x - 22 > 0 && x - 23 < map.x() && aux_y - 16 < map.y());
                bn::point grid_coord(bamod(x - 7, 32), bamod(current_y + 24, 32));

                if(not_oob){
                    int cell_index = x - 23 + (aux_y - 16)*map.x();
                    bg_m_ptr->set_cell(grid_coord, map[cell_index], map.horizontal_flip(cell_index));
                }else{
                    bg_m_ptr->set_cell(grid_coord, 0);
                }
            }
        }else if(current_y < prev_y){   // If moved Up
            for(int x = current_x; x < cx_pthtw; x++){
                bool not_oob = (x - 22 > 0 && x - 23 < map.x() && current_y - 15 > 0);
                bn::point grid_coord(bamod(x - 7, 32), bamod(current_y, 32));

                if(not_oob){
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

    // Init must be called ONCE before the loop begins
    void init(bn::camera_ptr const& cam){
        bg_m_ptr->reset();

        // Defining the MAP ARRAY bounds to redraw the map
        int intcam_x = cam.x().integer() , intcam_y = cam.y().integer();
        int current_x = (intcam_x)>>3    , current_y = (intcam_y + 47)>>3;
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
        update(cam);
    }
    
    game_map map;
private:
    int prev_x, prev_y;
    bn::unique_ptr<bg_map> bg_m_ptr;
    bn::regular_bg_ptr bg;
    bn::regular_bg_map_ptr bg_m;
};

}

#endif