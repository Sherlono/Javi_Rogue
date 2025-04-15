#include "jv_environment.h"

namespace jv{
// Make all block prefabs here
void BlockFactory(game_map& map, const bn::point top_left, const uchar_t option, const bool blockFlip){
    uchar_t arr[16];
    for(int i = 0; i < 16; i++){ arr[i] = jv::blocks::block_array[option < B_COUNT ? option : 0][i];}

    game_map blk(4, 4, arr);
    map.insert_map(blk, top_left, blockFlip);
}

// This function is meant to be replaced with procedural generation
void LevelFactory(game_map& map, const int option){
    int width, height;
    map.reset();
    
    switch(option){
        case 1:{
            width = 20;
            height = 20;
            uchar_t blockArr[400] = {
                 0, 0,31,28,31, 0, 0, 0, 0,31,28,28,28,28,28,31, 0, 0, 0, 0,
                36,25,29,22,29,25,28,28,25,29,22,22,22,22,22,29,34, 0, 0, 0,
                35,24,32, 4,32,24,22,22,24,32, 3, 2, 2, 2, 3,33,38,28,31, 0,
                35,21,18,20,18, 6, 2, 2, 6,18,20,20,20,20,20, 1,22,22,29,34,
                35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 8, 8,33,35,
                35, 1,20,20,20,20,20,13,10,20,10,13,20,20,20, 1,42,42, 7,35,
                37,17,11,10,20,10,11,17,38, 7,42,17,11,20,11,17,37,35, 7,35,
                 0,41,40,42, 7,22,22,22,22, 7,35,41,42, 7,42,41, 0,35, 7,35,
                 0, 0, 0,35,20, 8, 8, 8, 8,20,38,28,38, 7,35, 0, 0,35, 7,35,
                 0, 0, 0,35, 7,42,40,40,42, 7,22,22,22, 7,35, 0, 0,35, 7,35,
                 0, 0, 0,35, 7,35, 0,31,38,20, 8, 8, 8,20,35, 0, 0,35, 7,35,
                 0, 0, 0,35, 7,35,34,29,22, 7,22,29,42, 7,35, 0, 0,35, 7,35,
                 0,31,28,38, 7,38,38,33, 3,20, 3,33,39, 7,38,28,31,35, 7,35,
                34,29,22,22, 7,22,22, 1,20,20,20, 1,22, 7,22,22,29,35, 7,35,
                35,33, 3, 2,20, 2, 2, 5,20,20,20, 5, 2,20, 2, 3,33,39, 7,35,
                35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 1,22, 7,35,
                35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 8,17,37,
                35, 1,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 1,42,41, 0,
                37,17,11,10,10,10,10,10,10,10,10,10,10,10,10,11,17,37, 0, 0,
                 0,41,40,40,40,40,40,40,40,40,40,40,40,40,40,40,41, 0, 0, 0 };

            bool flipArr[400] = {
                 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
                 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
                 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
                 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
                 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
                 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
                 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
                 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
                 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1,
                 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1,
                 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
                 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };

            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    BlockFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                }
            }
            break;
            }
        case 2:{
            width = 10;
            height = 11;
            uchar_t blockArr[110] = {
                 0,31,28,28,28,28,28,28,31, 0,
                34,29,22,22,22,22,22,22,29,34,
                35,33, 3, 2, 2, 2, 2, 3,33,35,
                35, 1,20,20,20,20,20,20, 1,35,
                35, 1,20,20,20,20,20,20, 1,35,
                35, 1,20,20,20,20,20,20, 1,35,
                35, 1,20,20,20,20,20,20, 1,35,
                35, 1,20,20,20,20,20,20, 1,35,
                35, 1,20,20,20,20,20,20, 1,35,
                37,17,11,10,10,10,10,11,17,37,
                 0,41,40,40,40,40,40,40,41, 0 };
                 
            bool flipArr[110] = {
                0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 1, 0, 1, 0, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };

            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    BlockFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                }
            }
            break;
            }
        default:{
            width = 12;
            height = 10;
            uchar_t blockArr[120] = {
                 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
                 6, 6, 7, 7, 8, 8, 9, 9,10,10,11,11,
                12,12,13,13,14,14,15,15,16,16,17,17,
                18,18,19,19,20,20,21,21,22,22,23,23,
                24,24,25,25,26,26,27,27,28,28,29,29,
                30,30,31,31,32,32,33,33,34,34,35,35,
                36,36,37,37,38,38,39,39,40,40,41,41,
                42,42,43,43,44,44,45,45,46,46,47,47,
                48,48,49,49,50,51,52,53,54,55,56,57,
                58,59,60,61,62,63,64,65,66,67,68,69};
                
            bool flipArr[120] = {
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };

            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    BlockFactory(map, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
                }
            }
        }
        break;
    }
}

}