//#include "bn_random.h"

#include "jv_environment.h"
#include "jv_interface.h"

namespace jv{
// Make all floor prefabs here
void FloorFactory(game_map& map, const bn::point top_left, const uchar_t option, const bool blockFlip){
    bn::array<uchar_t, 16> block_array;

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
            block_array[3] += 127;
            block_array[7] += 127;
            block_array[11] += 127;
            block_array[15] += 127;
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
            block_array[2] += 127;
            block_array[3] += 127;
            block_array[6] += 127;
            block_array[7] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
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
            block_array[7] += 127;
            block_array[11] += 127;
            block_array[15] += 127;
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
            block_array[2] += 127;
            block_array[3] += 127;
            block_array[6] += 127;
            block_array[7] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
            break;
        }
        case 23:{
            block_array = {46, 47, 47, 46,
                           48, 49, 49, 48,
                           50, 51, 51, 50,
                           48, 49, 49, 48};
            block_array[2] += 127;
            block_array[3] += 127;
            block_array[6] += 127;
            block_array[7] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
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
            block_array[2] += 127;
            block_array[3] += 127;
            block_array[6] += 127;
            block_array[7] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
            break;
        }
        case 28:{
            block_array = { 0,  0,  0,  0,
                            0,  0,  0,  0,
                           46, 47, 47, 46,
                           48, 49, 49, 48};
            block_array[2] += 127;
            block_array[3] += 127;
            block_array[6] += 127;
            block_array[7] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
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
            block_array[0] += 127;
            block_array[1] += 127;
            block_array[4] += 127;
            block_array[5] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
            break;
        }
        case 39:{
            block_array = {82, 81, 81, 82,
                           84, 83, 83, 84,
                           46, 47, 47, 46,
                           48, 49, 49, 48};
            block_array[0] += 127;
            block_array[1] += 127;
            block_array[4] += 127;
            block_array[5] += 127;
            block_array[10] += 127;
            block_array[11] += 127;
            block_array[14] += 127;
            block_array[15] += 127;
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
            block_array[0] += 127;
            block_array[1] += 127;
            block_array[4] += 127;
            block_array[5] += 127;
            block_array[8] += 127;
            block_array[9] += 127;
            block_array[12] += 127;
            block_array[13] += 127;
            break;
        }
        // ********************************
        default:{
            block_array = {0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0,
                           0,  0,  0,  0};
            break;
        }
    }

    game_map piece(4, 4, block_array.data());
    map.insert_map(piece, top_left, blockFlip);
}


}