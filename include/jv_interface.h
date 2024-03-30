#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_math.h"
#include "bn_vector.h"

#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

namespace jv{
struct Mapinfo{
    Mapinfo(cuchar_t x, cuchar_t y, cuchar_t* arr):width(x), height(y), size(x*y), _arr(arr){}
    unsigned char operator[](unsigned int index) const{
        if(index < size){
            return _arr[index];
        }else{
            return 0;   // Not perfect but works if no radical changes are made to how block constructors work
        }
    }
    // Members
    cuchar_t width, height, size;
private:
    cuchar_t* _arr;
};

void LevelGenerator(const Mapinfo map, bn::camera_ptr& cam, bn::vector<jv::para, jv::ct::max_blocks>& para_vector, bn::vector<jv::Block*, jv::ct::max_blocks>& block_holder){
    // Did the player move enough to load assets
    static int prev_x = 999, prev_y = 999;
    int current_x = (cam.x()/48).round_integer(), current_y = (cam.y()/32).round_integer();
    
    if(current_x != prev_x || current_y != prev_y){
        // Clear map to make anew
        for(int b = 0; b < para_vector.size(); b++){
            delete block_holder[b];
        }
        para_vector.clear();
        block_holder.clear();

        // Defining the MAP ARRAY bounds to redraw the map
        int aux_x1 = ((cam.x() - 192)/32).round_integer(), aux_y1 = ((cam.y() - 128)/32).round_integer();
        int aux_x2 = ((cam.x() + 192)/32).round_integer(), aux_y2 = ((cam.y() + 128)/32).round_integer();
        unsigned int left_bound = aux_x1 * (aux_x1 > 0), right_bound = aux_x2 * (aux_x2 <= map.width) + map.width * (aux_x2 > map.width);
        unsigned int top_bound = aux_y1 * (aux_y1 > 0), bottom_bound = aux_y2 * (aux_y2 <= map.height) + map.height * (aux_y2 > map.height);

        // Redraw map inside bounds
        for(size_t y = top_bound; y < bottom_bound; y++){
            for(size_t x = left_bound; x < right_bound; x++){

                unsigned char index = x + y * map.width;

                if(map[index] && map[index] <= jv::ct::block_type_count){   // if block type is not 0 and is not above block type count
                    jv::Block* newblock;
                    if(map[index] <= jv::ct::w1count){
                        newblock = new Wall1(x*32, y*32, cam, map[index], jv::ct::max_blocks - y);
                    }else if(map[index] <= jv::ct::w1w2count){
                        newblock = new Wall2(x*32, y*32, cam, map[index], jv::ct::max_blocks - y);
                    }else{
                        newblock = new Floor(x*32, y*32, cam, map[index], jv::ct::max_blocks - y);
                    }
                    para_vector.push_back(newblock->get_para());
                    block_holder.push_back(newblock);
                }
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
}
}

#endif