#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

//#include "bn_random.h"
#include "bn_array.h"

namespace jv{
void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){
        bn::core::reset();
    }
}

struct GameMap{
    // Methods
    GameMap(cuchar_t x, cuchar_t y, uchar_t* arr):width(x), height(y), _arr(arr){}
    unsigned char operator[](const unsigned int index) const{
        if(index < width*height){
            return _arr[index];
        }else{
            return 0;   // Not perfect but works if no radical changes are made to how block constructors work
        }
    }
    // Insert room into the main map starting by the top left corner
    void insert_room(GameMap room, const bn::point top_left){
        int y_begin = top_left.y()  ,   x_begin = top_left.x();
        int aux_y = y_begin + room.height   ,   aux_x = x_begin + room.width;
        int y_end = aux_y * (aux_y < height) + height * (aux_y >= height)   ,   x_end = aux_x * (aux_x < width) + width * (aux_x >= width);

        for(int y = y_begin; y < y_end; y++){
            for(int x = x_begin; x < x_end; x++){
                int map_index = x + y * this->width;
                int room_index = (x - x_begin) + (y - y_begin) * room.width;
                this->_arr[map_index] =  room._arr[room_index];
            }
        }
    }
    // Members
    uchar_t width, height;
    uchar_t* _arr;
};

// Make all room prefabs here
GameMap RoomPrefab(unsigned char option){
    switch(option){
        default:{    // All environment assets layed out
            static uchar_t block_array[82] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,
                                              10, 11, 12, 13, 14, 15, 16, 17, 18,
                                              19, 20, 21, 22, 23, 24, 25, 26, 27,
                                              28, 29, 30, 31, 32, 33, 34, 35, 36,
                                              37, 38, 39, 40, 41, 42, 43, 44, 45,
                                              46, 47, 48, 49, 50, 51, 52, 53, 54,
                                              55, 56, 57, 58, 59, 60, 61, 62, 63,
                                              64, 65, 66, 67, 68, 69, 70, 71, 72,
                                              73, 74, 75, 76, 77, 78, 79, 80, 81};
            return GameMap(9, 10, block_array);
        }
    }
}

// Make all floor prefabs here
void FloorFactory(const bn::point top_left, unsigned char option, bool blockFlip, bn::unique_ptr<bg_map>& bg_map_ptr){
    static bn::array<uint16_t, 16> block_array;
    static bn::array<bool, 16> flip_array;
    flip_array = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch(option){
        case 1:{
            block_array = {1,  2,  3,  15,
                           4,  5,  6,  15,
                           1,  2,  3,  15,
                           4,  5,  6,  15};
            break;
        }
        case 2:{
            block_array = {11, 10, 10, 11,
                           19, 19, 19, 19,
                           15, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        case 3:{
            block_array = { 9, 10, 10, 11,
                           18, 19, 19, 19,
                           27, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        case 4:{
            block_array = { 9, 10, 10,  9,
                           18, 19, 19, 18,
                           27, 15, 15, 27,
                           15, 15, 15, 15,};
            flip_array[3] = true;
            flip_array[7] = true;
            flip_array[11] = true;
            flip_array[15] = true;
            break;
        }
        case 5:{
            block_array = {12, 13, 14, 15,
                           26, 27, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        case 6:{
            block_array = {16, 17, 18, 26,
                           18, 26, 27, 15,
                           27, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        case 7:{
            block_array = { 0,  0,  7, 8,
                            7,  8, 16, 17,
                           16, 17, 18, 26,
                           18, 26, 27, 15};
            break;
        }
        case 8:{
            block_array = { 0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 7, 8,
                            7, 8,16,17};
            break;
        }
        case 9:{
            block_array = { 0,  0,  7,  8,
                            7,  8, 16, 17,
                           23, 24, 25, 26,
                            4,  5,  6, 15};
            break;
        }
        case 10:{
            block_array = {15, 15, 15, 15,
                           15, 15, 15, 15,
                           20, 20, 20, 20,
                           29, 28, 28, 29};
            break;
        }
        case 11:{
            block_array = {15, 15, 15, 15,
                           22, 15, 15, 15,
                           44, 20, 20, 20,
                           39, 28, 28, 29};
            break;
        }
        case 12:{
            block_array = {15, 15, 15, 15,
                           22, 15, 15, 22,
                           44, 20, 20, 44,
                           39, 28, 28, 39};
            flip_array[7] = true;
            flip_array[11] = true;
            flip_array[15] = true;
            break;
        }
        case 13:{
            block_array = {15, 15, 15, 15,
                           15, 15, 15, 15,
                           45, 22, 15, 15,
                           30, 31, 32, 15};
            break;
        }
        case 14:{
            block_array = {15, 15, 15, 15,
                           22, 15, 15, 15,
                           44, 45, 22, 15,
                           42, 43, 44, 45};
            break;
        }
        case 15:{
            block_array = {44, 45, 22, 15,
                           42, 43, 44, 45,
                           37, 38, 42, 43,
                            0,  0, 37, 38};
            break;
        }
        case 16:{
            block_array = {37, 38, 42, 43,
                            0,  0, 37, 38,
                            0,  0,  0,  0,
                            0,  0,  0,  0};
            break;
        }
        case 17:{
            block_array = { 1,  2,  3, 15,
                           34, 35, 36, 45,
                           37, 38, 42, 43,
                            0,  0, 37, 38};
            break;
        }
        case 18:{
            block_array = {27, 15, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        case 19:{
            block_array = {15, 15, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15,
                           22, 15, 15, 15};
            break;
        }
        case 20:{
            block_array = {15, 15, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15,
                           15, 15, 15, 15};
            break;
        }
        default:{
            block_array = {0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0};
            break;
        }
    }
    
    bg_map_ptr->insert_block(4, 4, block_array, flip_array, blockFlip, top_left);
}

namespace LevelMaker{
// Init must be called Once
/*void init(const GameMap map, bn::camera_ptr& cam, bn::vector<bn::sprite_ptr, MAX_BLOCKS>& block_vector){
    // Defining the MAP ARRAY bounds to redraw the map
    int aux_x1 = (cam.x().integer() - 144)/32;      // Horizontal block load bound
    int aux_y1 = (cam.y().integer() - 96)/32;      // Vertical block load bound
    int left_bound = aux_x1 * (aux_x1 > 0)  ,   right_bound = left_bound + 11;
    int top_bound = aux_y1 * (aux_y1 > 0)   ,   bottom_bound = top_bound + 7;

    // Redraw map inside bounds
    for(int y = top_bound; y < bottom_bound; y++){
        for(int x = left_bound; x < right_bound; x++){
            unsigned int index = x + y * map.width;

            //jv::Block* newblock;
            bool not_outside = (x < map.width && y < map.height);
            //newblock = new jv::Block(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
            //block_vector.push_back(newblock->get_sprite_ptr());
            //delete newblock;
        }
    }
}

// Update must be run every frame
static int prev_x = 0, prev_y = 0;
void update(const GameMap map, bn::camera_ptr& cam, bn::vector<bn::sprite_ptr, MAX_BLOCKS>& block_vector){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer())/48  ,   current_y = (cam.y().integer())/32;
    
    if(current_x != prev_x || current_y != prev_y){
        // Defining the MAP ARRAY bounds to redraw the map
        int aux_x1 = (cam.x().integer() - 144)/32;      // Horizontal block load bound
        int aux_y1 = (cam.y().integer() - 96)/32;      // Vertical block load bound
        int left_bound = aux_x1 * (aux_x1 > 0)  ,   right_bound = left_bound + 11;
        int top_bound = aux_y1 * (aux_y1 > 0)   ,   bottom_bound = top_bound + 7;

        // Redraw map inside bounds
        unsigned int i = 0;
        for(int y = top_bound; y < bottom_bound; y++){
            for(int x = left_bound; x < right_bound; x++){
                unsigned int index = x + y * map.width;
                bool not_outside = (x < map.width && y < map.height);
                //jv::Block newblock(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
                //block_vector[i] = newblock.get_sprite_ptr();
                i++;
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
}*/
}
}

#endif