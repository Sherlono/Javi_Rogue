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

    cuchar_t width, height, size;
private:
    cuchar_t* _arr;
};

void LevelGenerator(const Mapinfo map, bn::camera_ptr& cam, bn::vector<jv::para, jv::ct::max_blocks>& para_vector, bn::vector<jv::Block*, jv::ct::max_blocks>& block_holder){
    static int prev_x = 999, prev_y = 999;
    int current_x = (cam.x()/16).round_integer(), current_y = (cam.y()/16).round_integer();
    
    if(current_x != prev_x || current_y != prev_y){
        // Clear map to make anew
        for(int b = 0; b < para_vector.size(); b++){
            delete block_holder[b];
        }
        para_vector.clear();
        block_holder.clear();

        // Redraw map inside window
        int i = 0;
        for(size_t y = 0; y < map.height; y++){
            for(size_t x = 0; x < map.width; x++){
                if(map[i] && map[i] <= jv::ct::block_type_count){
                    int x32 = x * 32, y32 = y * 32;
                    bool in_screen = x32 >= cam.x() - 180 && x32 <= cam.x() + 180 && y32 >= cam.y() - 125 && y32 <= cam.y() + 125;
                    if(in_screen){      // Create block
                        jv::Block* newblock;
                        if(map[i] <= jv::ct::w1count){
                            newblock = new Wall1(x32, y32, cam, map[i], y);
                        }else if(map[i] <= jv::ct::w1w2count){
                            newblock = new Wall2(x32, y32, cam, map[i], y);
                        }else{
                            newblock = new Floor(x32, y32, cam, map[i], y);
                        }
                        para_vector.push_back(newblock->get_para());
                        block_holder.push_back(newblock);
                    }
                }
                i++;
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
}
}

#endif