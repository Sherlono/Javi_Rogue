#include "jv_environment.h"

//#include "bn_random.h"

namespace jv{
// Make all floor prefabs here
void FloorFactory(game_map& map, const bn::point top_left, const uchar_t option, const bool blockFlip){
    static uchar_t b_a[16];
    static bn::array<uchar_t, 16> block_array;
    //static bn::array<bool, 16> flip_array;
    static bool flip_array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

    for(int i = 0; i < 16; i++){
        b_a[i] = block_array[i];
    }

    game_map piece(4, 4, b_a, flip_array);
    map.insert_room(piece, top_left, blockFlip);
}
}