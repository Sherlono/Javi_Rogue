#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_math.h"

#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

namespace jv{
    void LevelGenerator(unsigned char width, unsigned char height, bn::camera_ptr& cam, bn::vector<jv::para, constants::max_blocks>& para_vector, bn::vector<jv::Block*, constants::max_blocks>& block_holder, unsigned char* block_array){
        unsigned char i = 0;
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                if(block_array[i] <= 64){
                    jv::Block* newblock;
                    if(block_array[i] <= 16){           // 0 - 16
                        newblock = new Wall1(x, y, cam, block_array[i], y);
                    }else if(block_array[i] <= 21){     // 17 - 21
                        newblock = new Wall2(x, y, cam, block_array[i] - 17, y);
                    }else{                              // 22 - 60
                        newblock = new Floor(x, y, cam, block_array[i] - 22, y);
                    }
                    para_vector.push_back(newblock->get_para());
                    block_holder.push_back(newblock);
                }
                i++;
            }
        }
    }
}

#endif