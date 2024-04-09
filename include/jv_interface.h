#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

//#include "bn_random.h"

namespace jv{
void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){
        bn::core::reset();
    }
}
/*int block_scroll(jv::Block* myblock, bn::camera_ptr& cam){
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
}*/

struct GameMap{
    GameMap(cuchar_t x, cuchar_t y, uchar_t* arr):width(x), height(y), size(x*y), _arr(arr){}
    unsigned char operator[](unsigned int index) const{
        if(index < size){
            return _arr[index];
        }else{
            return 0;   // Not perfect but works if no radical changes are made to how block constructors work
        }
    }
    // Members
    cuchar_t width, height, size;
    uchar_t* _arr;
};
// Insert room into the main map starting by the top left corner
void insert_room(jv::GameMap room, jv::GameMap& map, bn::point top_left){
    uchar_t y_begin = top_left.y()  ,   y_end = y_begin + room.height;
    uchar_t x_begin = top_left.x()  ,   x_end = x_begin + room.width;
    for(uchar_t y = y_begin; y < y_end; y++){
        for(uchar_t x = x_begin; x < x_end; x++){
            int map_index = x + y * map.width;
            int room_index = (x - x_begin) + ((y - y_begin) * room.width);
            map._arr[map_index] = room._arr[room_index];
        }
    }
}

GameMap RoomGenerator(unsigned char option){
    switch(option){
        case 1:{
            bn::point map_shape(6, 5);
            uchar_t block_array[30] = {1 , 24, 28, 28, 25, 2 ,
                                       5 , 29, 37, 38, 30, 6 ,
                                       7 , 51, 53, 54, 78, 8 ,
                                       9 , 45, 55, 53, 46, 10,
                                       11, 15, 21, 22, 16, 12};
            return jv::GameMap(map_shape.x(), map_shape.y(), block_array);
        }
        default:{    // All environment assets layed out
            bn::point map_shape(9, 10);
            uchar_t block_array[90] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,
                                       10, 11, 12, 13, 14, 15, 16, 17, 18,
                                       19, 20, 21, 22, 23, 24, 25, 26, 27,
                                       28, 29, 30, 31, 32, 33, 34, 35, 36,
                                       37, 38, 39, 40, 41, 42, 43, 44, 45,
                                       46, 47, 48, 49, 50, 51, 52, 53, 54,
                                       55, 56, 57, 58, 59, 60, 61, 62, 63,
                                       64, 65, 66, 67, 68, 69, 70, 71, 72,
                                       73, 74, 75, 76, 77, 78, 79, 80, 81};
            return jv::GameMap(map_shape.x(), map_shape.y(), block_array);
        }
    }
}
/*GameMap GameMapGenerator(const unsigned int x, const unsigned int y, bn::random randomizer){

}*/

namespace LevelMaker{
// Init must be called Only Once
void init(const GameMap map, bn::camera_ptr& cam, bn::vector<jv::para, MAX_PARA>& para_vector, bn::vector<bn::sprite_ptr, MAX_BLOCKS>& block_v){
    para_vector.clear();
    for(int y = 0; y < map.height; y++){
        for(int x = 0; x < map.width; x++){
            unsigned int index = x + y * map.width;

            jv::Block newblock(x*32, y*32, cam, map[index], MAX_BLOCKS - y);
            if(map[index] <= W_COUNT){
                jv::para newpara = newblock.get_para();
                para_vector.push_back(newpara);
            }
        }
    }
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
            bool not_outside = (x < map.width && y < map.height);
            newblock = new jv::Block(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
            block_v.push_back(newblock->get_sprite_ptr());
            delete newblock;
        }
    }
}

// Update must be run every frame
static int prev_x = 0, prev_y = 0;
void update(const GameMap map, bn::camera_ptr& cam, bn::vector<bn::sprite_ptr, MAX_BLOCKS>& block_v){
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
                jv::Block newblock(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
                block_v[i] = newblock.get_sprite_ptr();
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