#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_math.h"
#include "bn_vector.h"
#include "bn_unordered_set.h"

#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

namespace jv{
struct GameMap{
    GameMap(cuchar_t x, cuchar_t y, cuchar_t* arr):width(x), height(y), size(x*y), _arr(arr){}
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

int block_scroll(jv::Block* myblock, bn::camera_ptr& cam){
    static int i = 0;
    if(bn::keypad::r_pressed()){
        if(i >= 0 && i < W_COUNT){
            delete myblock;
            i++;
            myblock = new jv::Wall(0, 0, cam, i);
        }else if(i < BLOCK_TYPE_COUNT){
            delete myblock;
            i++;
            myblock = new jv::Floor(0, 0, cam, i);
        }
    }else if(bn::keypad::l_pressed()){
        if(i > W_COUNT + 1){
            delete myblock;
            i--;
            myblock = new jv::Floor(0, 0, cam, i);
        }else if(i > 0){
            delete myblock;
            i--;
            myblock = new jv::Wall(0, 0, cam, i);
        }
    }
    return i;
}

namespace LevelMaker{
// Update must be run every frame
void update(const GameMap map, bn::camera_ptr& cam, bn::vector<para, MAX_BLOCKS>& para_vector, bn::vector<Block*, MAX_BLOCKS>& block_holder){
    // Did the player move enough to load assets
    static int prev_x = 999, prev_y = 999;
    int current_x = (cam.x().integer())/48, current_y = cam.y().integer()/32;
    
    if(current_x != prev_x || current_y != prev_y){
        // Defining the MAP ARRAY bounds to redraw the map
        int aux_x1 = (cam.x().integer() - 176)/32   ,   aux_x2 = (cam.x().integer() + 208)/32;                              // Horizontal block load bound not truncated to map width
        int aux_y1 = (cam.y().integer() - 128)/32   ,   aux_y2 = (cam.y().integer() + 128 + 32)/32;                              // Vertical block load bound not truncated to map height
        int left_bound = aux_x1 * (aux_x1 > 0)  ,   right_bound = aux_x2 * (aux_x2 <= map.width) + map.width * (aux_x2 > map.width);        // Truncated final result
        int top_bound = aux_y1 * (aux_y1 > 0)   ,   bottom_bound = aux_y2 * (aux_y2 <= map.height) + map.height * (aux_y2 > map.height);    // Truncated final result

        // Clear map to make anew
        para_vector.clear();
        for(int b = 0; b < block_holder.size(); b++){
            delete block_holder[b];
        }
        block_holder.clear();

        // Redraw map inside bounds
        for(int y = top_bound; y < bottom_bound; y++){
            for(int x = left_bound; x < right_bound; x++){
                unsigned char index = x + y * map.width;

                if(map[index] && map[index] <= BLOCK_TYPE_COUNT){   // if block type is not 0 and is not above block type count
                    Block* newblock;
                    if(map[index] <= W_COUNT){
                        newblock = new Wall(x*32, y*32, cam, map[index], MAX_BLOCKS - y);
                    }else{
                        newblock = new Floor(x*32, y*32, cam, map[index], MAX_BLOCKS - y);
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
}

#endif