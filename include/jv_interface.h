#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

//#include "bn_random.h"

namespace jv{
void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){
        bn::core::reset();
    }
}

// For debug purposes only
/*static int i = 0;
int block_scroll(jv::Block* myblock, bn::camera_ptr& cam){
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
    // Methods
    GameMap():width(0), height(0), size(0){}
    GameMap(cuchar_t x, cuchar_t y, uchar_t* arr):width(x), height(y), size(x*y), _arr(arr){}
    unsigned char operator[](const unsigned int index) const{
        if(index < size){
            return _arr[index];
        }else{
            return 0;   // Not perfect but works if no radical changes are made to how block constructors work
        }
    }
    // Insert room into the main map starting by the top left corner
    void insert_room(GameMap room, const bn::point top_left){
        int y_begin = top_left.y()  ,   y_end = y_begin + room.height;
        int x_begin = top_left.x()  ,   x_end = x_begin + room.width;
        for(int y = y_begin; y < y_end; y++){
            for(int x = x_begin; x < x_end; x++){
                int map_index = x + y * this->width;
                int room_index = (x - x_begin) + (y - y_begin) * room.width;
                this->_arr[map_index] =  room._arr[room_index];
            }
        }
    }
    // Members
    uchar_t width, height, size;
    uchar_t* _arr;
};
// Make all room prefabs here
GameMap RoomPrefab(unsigned char option){
    switch(option){
        case 1:{
            static uchar_t block_array[35] = {1 , 24, 28, 28, 28, 25, 2 ,
                                              5 , 29, 37, 49, 38, 30, 6 ,
                                              7 , 51, 53, 53, 54, 52, 8 ,
                                              9 , 45, 55, 53, 53, 46, 10,
                                              11, 15, 21, 20, 22, 16, 12};
            return GameMap(7, 5, block_array);
        }
        case 2:{
            static uchar_t block_array[63] = {1 , 24, 28, 28, 28, 25, 2 ,
                                              5 , 29, 37, 49, 38, 30, 6 ,
                                              5 , 51, 53, 54, 53, 52, 6 ,
                                              5 , 51, 55, 53, 53, 52, 6 ,
                                              5 , 51, 53, 53, 54, 52, 6 ,
                                              5 , 51, 53, 55, 53, 52, 6 ,
                                              7 , 51, 54, 53, 55, 52, 8 ,
                                              9 , 45, 53, 53, 53, 46, 10,
                                              11, 15, 21, 20, 22, 16, 12};
            return GameMap(7, 9, block_array);
        }
        case 3:{
            static uchar_t block_array[65] = {1 , 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 25, 2 ,
                                              5 , 29, 37, 49, 49, 49, 49, 49, 49, 49, 38, 30, 6 ,
                                              7 , 51, 54, 53, 53, 53, 53, 53, 54, 53, 53, 52, 8 ,
                                              9 , 45, 55, 53, 53, 55, 53, 55, 53, 53, 55, 46, 10,
                                              11, 15, 21, 20, 20, 20, 20, 20, 20, 20, 22, 16, 12};
            return GameMap(13, 5, block_array);
        }
        default:{    // All environment assets layed out
            static uchar_t block_array[90] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,
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

/*GameMap GameMapGenerator(const unsigned int x, const unsigned int y, bn::random randomizer){

}*/

namespace LevelMaker{
// Init must be called Once
void init(const GameMap map, bn::camera_ptr& cam, bn::vector<jv::para, MAX_PARA>& para_vector, bn::vector<bn::sprite_ptr, MAX_BLOCKS>& block_vector){
    para_vector.clear();
    block_vector.clear();
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
            block_vector.push_back(newblock->get_sprite_ptr());
            delete newblock;
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
                jv::Block newblock(x*32, y*32, cam, map[index] * not_outside, MAX_BLOCKS - y);
                block_vector[i] = newblock.get_sprite_ptr();
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