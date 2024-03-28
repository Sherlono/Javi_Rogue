#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_math.h"
#include "bn_vector.h"

#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

namespace jv{
void LevelGenerator(const unsigned int width, const unsigned int height, bn::camera_ptr& cam, bn::vector<jv::para, constants::max_blocks>& para_vector, bn::vector<jv::Block*, constants::max_blocks>& block_holder, const unsigned char* block_array){
    static unsigned int prev_x = 999, prev_y = 999;
    unsigned int current_x = (cam.x()/16).round_integer(), current_y = (cam.y()/16).round_integer();
    
    if(current_x != prev_x || current_y != prev_y){
        for(int b = 0; b < para_vector.size(); b++){
            delete block_holder[b];
        }
        para_vector.clear();
        block_holder.clear();

        int i = 0;
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                if(block_array[i] && block_array[i] <= constants::block_type_count){
                    int x32 = x * 32, y32 = y * 32;
                    bool in_screen = x32 >= cam.x() - 180 && x32 <= cam.x() + 180 && y32 >= cam.y() - 125 && y32 <= cam.y() + 125;
                    if(in_screen){      // Create block
                        jv::Block* newblock;
                        if(block_array[i] <= 23){           // 1 - 22
                            newblock = new Wall1(x32, y32, cam, block_array[i], y);
                        }else if(block_array[i] <= 28){     // 23 - 27
                            newblock = new Wall2(x32, y32, cam, block_array[i] - 23, y);
                        }else{                              // 28 - 78
                            newblock = new Floor(x32, y32, cam, block_array[i] - 28, y);
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