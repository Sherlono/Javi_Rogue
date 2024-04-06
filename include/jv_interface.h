#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_math.h"
#include "bn_vector.h"
#include "bn_random.h"

#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

namespace jv{
int block_scroll(jv::Block* myblock, bn::camera_ptr& cam){
    static int i = 0;
    if(bn::keypad::r_pressed()){
        if(i < BLOCK_TYPE_COUNT){
            delete myblock;
            i++;
            myblock = new jv::Block(0, 0, cam, i);
        }
    }else if(bn::keypad::l_pressed()){
        if(i > 0){
            delete myblock;
            i--;
            myblock = new jv::Block(0, 0, cam, i);
        }
    }
    return i;
}

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

/*GameMap GameMapGenerator(const unsigned int x, const unsigned int y, bn::random randomizer;){

}*/

namespace LevelMaker{
// Init must be called Only Once
void init(const GameMap map, bn::camera_ptr& cam, bn::vector<para, MAX_BLOCKS>& para_vector, bn::vector<Block*, MAX_BLOCKS>& block_v){
    // Defining the MAP ARRAY bounds to redraw the map
    int aux_x1 = (cam.x().integer() - 144)/32;      // Horizontal block load bound
    int aux_y1 = (cam.y().integer() - 96)/32;      // Vertical block load bound
    int left_bound = aux_x1 * (aux_x1 > 0)  ,   right_bound = left_bound + 11;
    int top_bound = aux_y1 * (aux_y1 > 0)   ,   bottom_bound = top_bound + 7;

    // Redraw map inside bounds
    for(int y = top_bound; y < bottom_bound; y++){
        for(int x = left_bound; x < right_bound; x++){
            unsigned int index = x + y * map.width;

            jv::Block* newblock;
            bool not_outside = (x <= map.width && y <= map.height);
            newblock = new jv::Block(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
            if(map[index] <= W_COUNT){
                para_vector.push_back(newblock->get_para());
            }
            block_v.push_back(newblock);
        }
    }
}

// Update must be run every frame
void update(const GameMap map, bn::camera_ptr& cam, bn::vector<para, MAX_BLOCKS>& para_vector, bn::vector<Block*, MAX_BLOCKS>& block_v){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer())/48  ,   current_y = (cam.y().integer())/32;
    static int prev_x = current_x, prev_y = current_y;
    
    if(current_x != prev_x || current_y != prev_y){
        // Defining the MAP ARRAY bounds to redraw the map
        int aux_x1 = (cam.x().integer() - 144)/32;      // Horizontal block load bound
        int aux_y1 = (cam.y().integer() - 96)/32;      // Vertical block load bound
        int left_bound = aux_x1 * (aux_x1 > 0)  ,   right_bound = left_bound + 11;
        int top_bound = aux_y1 * (aux_y1 > 0)   ,   bottom_bound = top_bound + 7;

        para_vector.clear();

        // Redraw map inside bounds
        unsigned int i = 0;
        for(int y = top_bound; y < bottom_bound; y++){
            for(int x = left_bound; x < right_bound; x++){
                unsigned int index = x + y * map.width;
                block_v[i]->set_block(x*32, y*32, cam, map[index] * (x < map.width && y < map.height), MAX_BLOCKS - y);
                if(map[index] <= W_COUNT){
                    para_vector.push_back(block_v[i]->get_para());
                }
                i++;
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
}
}
}

#endif