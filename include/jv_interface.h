#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

<<<<<<< Updated upstream
#include "bn_sprite_text_generator.h"
=======
#include "bn_math.h"
>>>>>>> Stashed changes

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

<<<<<<< Updated upstream

namespace jv{
    void LevelGenerator(unsigned char width, unsigned char height, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, bn::vector<jv::Block*, 40>& block_holder, unsigned char* block_array){
=======
namespace jv{
    void LevelGenerator(unsigned char width, unsigned char height, bn::camera_ptr& cam, bn::vector<jv::para, 50>& para_vector, bn::vector<jv::Block*, 50>& block_holder, unsigned char* block_array){
>>>>>>> Stashed changes
        unsigned char i = 0;
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                jv::Block* newblock;
                if(block_array[i] <= 14){
<<<<<<< Updated upstream
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
=======
                    newblock = new Wall2(x, y, cam, block_array[i]);
                }else if(block_array[i] <= 18){
                    newblock = new Wall4(x, y, cam, block_array[i] - 15);
                }else if(block_array[i] <= 19){
                    newblock = new Wall6(x, y, cam, block_array[i] - 19);
                }else if(block_array[i] <= 23){
                    newblock = new Wall8(x, y, cam, block_array[i] - 20);
                }else{
                    newblock = new Floor(x, y, cam, block_array[i] - 24);
                }
                para_vector.push_back(newblock->get_para());
>>>>>>> Stashed changes
                block_holder.push_back(newblock);
                i++;
            }
        }
    }
}

#endif