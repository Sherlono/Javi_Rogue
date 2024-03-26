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
    unsigned int current_x = (cam.x()/32).round_integer(), current_y = (cam.y()/32).round_integer();
    
    if(current_x != prev_x && current_y != prev_y){
        int i = 0, j = 0;
        bn::point del_que[constants::max_blocks];
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                if(block_array[i] <= constants::block_type_count){
                    int x32 = x * 32, y32 = y * 32;
                    bool in_screen = x32 > cam.x() - 80 && x32 < cam.x() + 80 && y32 > cam.y() - 80 && y32 < cam.y() + 80;
                    if(in_screen){      // Create block
                        jv::Block* newblock;
                        if(block_array[i] <= 16){           // 0 - 16
                            newblock = new Wall1(x32, y32, cam, block_array[i], y);
                        }else if(block_array[i] <= 21){     // 17 - 21
                            newblock = new Wall2(x32, y32, cam, block_array[i] - 17, y);
                        }else{                              // 22 - 60
                            newblock = new Floor(x32, y32, cam, block_array[i] - 22, y);
                        }
                        para_vector.push_back(newblock->get_para());
                        block_holder.push_back(newblock);
                    }else{              // Add block to delete que
                        del_que[j] = bn::point(x32, y32);
                        j++;
                    }
                }
                i++;
            }
        }
        
        for(int block = 0; block < block_holder.size(); block++){                                                                       // Check every existing block
            for(int k = 0; k < j; k++){                                                                                                 // Compare with each coordinate due for deletion
                if(block_holder[block]->x() == del_que[k].x() && block_holder[block]->y() == del_que[k].y()){                           // If it coincides with coordinate for block due for deletion
                    for(int p = 0; p < para_vector.size(); p++){                                                                        // Search for its paralelogram
                        if(para_vector[p].x(true) == block_holder[block]->x() && para_vector[p].y(true) == block_holder[block]->y()){   // Delete if one is found
                            para_vector.erase(para_vector.begin() + p);
                            break;
                        }
                        p++;
                    }
                    delete block_holder[block];                       // Then delete block itself
                    block_holder.erase(block_holder.begin() + block);
                }
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
}
}

#endif