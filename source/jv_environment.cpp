//#include "bn_random.h"

#include "jv_environment.h"
#include "jv_interface.h"

namespace jv{
// Make all floor prefabs here
void FloorFactory(game_map& map, const bn::point top_left, const uchar_t option, const bool blockFlip){
    bn::array<uchar_t, 16> block_array;
    bool flip_array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    switch(option){
        // Floor Tiles
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
            block_array = {1,  2,  2,  1,
                           4,  5,  5,  4,
                           1,  2,  2,  1,
                           4,  5,  5,  4};
            flip_array[2] = true;
            flip_array[3] = true;
            flip_array[6] = true;
            flip_array[7] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 8:{
            block_array = {11, 10, 10, 11,
                           19, 19, 19, 19,
                           20, 20, 20, 20,
                           29, 28, 28, 29}; 
            break;
        }
        case 9:{
            block_array = {  1,  2,  3, 15,
                             4,  5,  6, 15,
                            12, 13, 14, 15,
                            26, 27, 15, 15 };
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
                           90, 90, 37, 38};
            break;
        }
        case 16:{
            block_array = { 37, 38, 42, 43,
                            90, 90, 37, 38,
                            93, 94, 90, 90,
                             0,  0, 93, 94};
            break;
        }
        case 17:{
            block_array = { 1,  2,  3, 15,
                           34, 35, 36, 45,
                           37, 38, 42, 43,
                           90, 90, 37, 38};
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
        case 21:{
            block_array = {23, 24, 25, 26,
                            4,  5,  6, 15,
                            1,  2,  3,  15,
                            4,  5,  6,  15 };
            break;
        }
        // Wall Tiles
        case 22:{
            block_array = {50, 51, 51, 50,
                           48, 49, 49, 48,
                           50, 51, 51, 50,
                           52, 53, 53, 52};
            flip_array[2] = true;
            flip_array[3] = true;
            flip_array[6] = true;
            flip_array[7] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 23:{
            block_array = {46, 47, 47, 46,
                           48, 49, 49, 48,
                           50, 51, 51, 50,
                           48, 49, 49, 48};
            flip_array[2] = true;
            flip_array[3] = true;
            flip_array[6] = true;
            flip_array[7] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 24:{
            block_array = {69, 70, 63, 64,
                           65, 66, 71, 72,
                           73, 74, 75, 76,
                           77, 78, 16, 17};
            break;
        }
        case 25:{
            block_array = { 0,  0, 54, 80,
                           57, 58, 59, 60,
                           61, 62, 63, 64,
                           65, 66, 67, 68};
            break;
        }
        case 26:{
            block_array = { 0,  0, 54, 56,
                           57, 58, 59, 60,
                           61, 62, 63, 64,
                           65, 66, 67, 68};
            break;
        }
        case 27:{
            block_array = {50, 51, 51, 50,
                           48, 49, 49, 48,
                           50, 51, 51, 50,
                           48, 49, 49, 48};
            flip_array[2] = true;
            flip_array[3] = true;
            flip_array[6] = true;
            flip_array[7] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 28:{
            block_array = { 0,  0,  0,  0,
                            0,  0,  0,  0,
                           46, 47, 47, 46,
                           48, 49, 49, 48};
            flip_array[2] = true;
            flip_array[3] = true;
            flip_array[6] = true;
            flip_array[7] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 29:{
            block_array = {61, 62, 63, 64,
                           65, 66, 67, 68,
                           69, 70, 63, 64,
                           65, 66, 71, 72};
            break;
        }
        case 30:{
            block_array = { 0,  0,  0,  0,
                            0,  0,  0,  0,
                            0,  0, 54, 80,
                           57, 58, 59, 60,};
            break;
        }
        case 31:{
            block_array = { 0,  0,  0,  0,
                            0,  0,  0,  0,
                            0,  0, 54, 56,
                           57, 58, 59, 60};
            break;
        }
        case 32:{
            block_array = {73, 74, 75, 76,
                           77, 78, 16, 17,
                           16, 17, 18, 26,
                           18, 26, 27, 15};
            break;
        }
        case 33:{
            block_array = {73, 74, 75, 76,
                           77, 78, 16, 17,
                           23, 24, 25, 26,
                            4,  5,  6, 15};
            break;
        }
        case 34:{
            block_array = { 0,  0, 79, 80,
                            0,  0, 81, 82,
                            0,  0, 83, 84,
                            0,  0, 81, 82};
            break;
        }
        case 35:{
            block_array = { 0,  0, 81, 82,
                            0,  0, 83, 84,
                            0,  0, 81, 82,
                            0,  0, 83, 84};
            break;
        }
        case 36:{
            block_array = { 0,  0,  0,  0,
                            0,  0,  0,  0,
                            0,  0, 79, 80,
                            0,  0, 83, 84 };
            break;
        }
        case 37:{
            block_array = { 0,  0, 81, 82,
                            0,  0, 85, 86,
                            0,  0,  0,  0,
                            0,  0,  0,  0 };
            break;
        }
        case 38:{
            block_array = {82, 81,  0,  0,
                           84, 83,  0,  0,
                           46, 47, 47, 46,
                           48, 49, 49, 48 };
            flip_array[0] = true;
            flip_array[1] = true;
            flip_array[4] = true;
            flip_array[5] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        case 39:{
            block_array = {82, 81, 81, 82,
                           84, 83, 83, 84,
                           46, 47, 47, 46,
                           48, 49, 49, 48};
            flip_array[0] = true;
            flip_array[1] = true;
            flip_array[4] = true;
            flip_array[5] = true;
            flip_array[10] = true;
            flip_array[11] = true;
            flip_array[14] = true;
            flip_array[15] = true;
            break;
        }
        // 
        case 40:{
            block_array = {90, 90, 90, 90,
                           90, 90, 90, 90,
                            0,  0,  0,  0,
                            0,  0,  0,  0};
            break;
        }
        case 41:{
            block_array = {93, 94, 90, 90,
                            0,  0, 93, 94,
                            0,  0,  0,  0,
                            0,  0,  0,  0};
            break;
        }
        case 42:{
            block_array = {90, 90, 91, 82,
                           90, 90, 92, 84,
                            0,  0, 81, 82,
                            0,  0, 83, 84};
            break;
        }
        case 43:{
            block_array = { 82, 81, 81, 82,
                            84, 83, 83, 84,
                            82, 81, 81, 82,
                            84, 83, 83, 84};
            flip_array[0] = true;
            flip_array[1] = true;
            flip_array[4] = true;
            flip_array[5] = true;
            flip_array[8] = true;
            flip_array[9] = true;
            flip_array[12] = true;
            flip_array[13] = true;
            break;
        }
        /*case 40:{
            block_array = {87, 88, 89, 90,
                           91, 92, 93, 94,
                           95, 96, 97, 98,
                           99,100,101,102};
            break;
        }*/
        // ********************************
        default:{
            block_array = {0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0};
            break;
        }
    }

    game_map piece(4, 4, block_array.data(), flip_array);
    map.insert_room(piece, top_left, blockFlip);
}


}