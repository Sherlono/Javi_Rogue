#include "jv_environment.h"

namespace jv{
// Make all block prefabs here
void BlockFactory(game_map& map, const bn::point top_left, const uint8_t option, const bool blockFlip){
    uint8_t arr[16];
    for(int i = 0; i < 16; i++){ arr[i] = jv::blocks::block_array[option < BLOCK_COUNT ? option : 0][i];}

    game_map blk(4, 4, arr);
    map.insert_map(blk, top_left, blockFlip);
}

bn::point InsertRoom(game_map& map, const bn::point top_left, const uint8_t option){
    switch(option){
        case 1:{
            const uint8_t width = 7, height = 7;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,31,28,28,28,31, 0,
                    34,29,22,22,22,29,34,
                    35,33, 3, 2, 3,33,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    37,17,11,10,11,17,37,
                     0,41,40,40,40,41, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 1, 1, 1, 1,
                     0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(1, 1);
        }
        case 2:{
            const uint8_t width = 7, height = 14;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,31,28,28,28,31, 0,
                    34,29,22,22,22,29,34,
                    35,33, 3, 2, 3,33,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    37,17,11,10,11,17,37,
                     0,41,40,40,40,41, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(1, 2);
        }
        case 3:{
            const uint8_t width = 14, height = 7;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,31,28,28,28,28,28,28,28,28,28,28,31, 0,
                    34,29,22,22,22,22,22,22,22,22,22,22,29,34,
                    35,33, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3,33,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    37,17,11,10,10,10,10,10,10,10,10,11,17,37,
                     0,41,40,40,40,40,40,40,40,40,40,40,41, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(2, 1);
        }
        case 4:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,31,28,28,28,28,28,28,28,28,28,28,31, 0,
                    34,29,22,22,22,22,22,22,22,22,22,22,29,34,
                    35,33, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3,33,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    37,17,11,10,10,10,10,10,10,10,10,11,17,37,
                     0,41,40,40,40,40,40,40,40,40,40,40,41, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(2, 2);
        }
        case 5:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,31,28,28,28,28,31, 0,31,28,28,28,31, 0,
                    34,29,22,22,22,22,29,43,29,22,22,22,29,34,
                    35,33, 3, 2, 2, 3,33,43,33, 3, 2, 3,33,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,39, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,22, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 5, 2, 5,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    35, 1,20,20,20,20,20,20,20,20,20,20, 1,35,
                    37,17,11,10,10,10,10,10,10,10,10,11,17,37,
                     0,41,40,40,40,40,40,40,40,40,40,40,41, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(2, 2);
        }
        case 6:{
            const uint8_t width = 3, height = 5;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,20, 0,
                    42, 7,42,
                    38, 7,38,
                    22, 7,22,
                     0,20, 0 };
                bool flipArr[size] = {
                     0, 0, 1,
                     0, 0, 1,
                     1, 0, 0,
                     0, 0, 1,
                     0, 0, 1 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(0, 0);
        }
        case 7:{
            const uint8_t width = 4, height = 4;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                     0,28,28, 0,
                     0,22,22, 0,
                     0, 8, 8, 0,
                     0,40,40, 0 };
                bool flipArr[size] = {
                     0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0, 0, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(0, 0);
        }
        default:{
            const uint8_t width = 4, height = 4;
            constexpr uint16_t size = width*height;

            if(true){
                uint8_t blockArr[size] = {
                    20, 0,20, 0,
                     0,20, 0,20,
                    20, 0,20, 0,
                     0,20, 0,20};
                bool flipArr[size] = {
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        BlockFactory(map, bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                    }
                }
            }
            return bn::point(0, 0);
        }
    }
}

void GenerateLevel(game_map& map, bn::random& randomizer){
    map.reset();
    bool sectors[3][3] = {{0, 0, 0},
                          {0, 0, 0},
                          {0, 0, 0}};

    // Creating rooms
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            if(sectors[y][x] == true){ continue;}
            
            bn::vector<uint8_t, ROOM_COUNT> validRooms;
            bn::point top_left(x, y);

            validRooms.push_back(1);
            if(y + 1 != 3){
                if(!sectors[y+1][x]){
                    validRooms.push_back(2);
                }
            }
            if(x + 1 != 3){
                if(!sectors[y][x+1]){
                    validRooms.push_back(3);
                }
            }
            if(x + 1 != 3 && y + 1 != 3){
                if(!sectors[y][x+1] && !sectors[y+1][x] && !sectors[y+1][x+1]){
                    validRooms.push_back(4);
                    validRooms.push_back(5);
                }
            }

            uint8_t option = validRooms[randomizer.get_int(0, validRooms.size())];
            bn::point occupied = InsertRoom(map, top_left*7, option);

            for(int row = y; row < y + occupied.y(); row++){
                for(int column = x; column < x + occupied.x(); column++){
                    sectors[row][column] = true;
                }
            }
        }
    }
    // Vertical corridors
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < 3; x++){
            if(map.cell((2 + x*7)*4, (6 + y*7)*4 + 1) == 0){ 
                InsertRoom(map, bn::point(2 + x*7, 5 + y*7), 6);
            }
        }
    }
    // Horizontal corridors
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 2; x++){
            if(map.cell((6 + x*7)*4 + 1, (2 + y*7)*4) == 0){
                InsertRoom(map, bn::point(5 + x*7, 2 + y*7), 7);
            }
        }
    }
}

}