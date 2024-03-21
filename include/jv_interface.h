#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_sprite_text_generator.h"

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"


namespace jv{
    void LevelGenerator(unsigned char width, unsigned char height, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, bn::vector<jv::Block*, 40>& block_holder, unsigned char* block_array){
        unsigned char i = 0;
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                jv::Block* newblock;
                if(block_array[i] <= 14){
                    newblock = new Wall2(x, y, cam, para_vector, block_array[i]);
                }else if(block_array[i] <= 18){
                    newblock = new Wall4(x, y, cam, para_vector, block_array[i] - 15);
                }else if(block_array[i] <= 19){
                    newblock = new Wall6(x, y, cam, para_vector);
                }else if(block_array[i] <= 23){
                    newblock = new Wall8(x, y, cam, para_vector, block_array[i] - 20);
                }else{
                    newblock = new Floor(x, y, cam, block_array[i] - 24);
                }
                block_holder.push_back(newblock);
                i++;
            }
        }
    }
}

#endif