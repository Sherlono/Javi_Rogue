#include "jv_environment.h"

namespace jv{
// Make all block prefabs here
void BlockFactory(game_map& map, const bn::point top_left, const uint8_t option, const bool blockFlip){
    uint8_t arr[16];
    for(int i = 0; i < 16; i++){
        arr[i] = jv::blocks::block_array[option < BLOCK_COUNT ? option : 0][i];
    }

    game_map blk(4, 4, arr);
    map.insert_map(blk, top_left, blockFlip);
}

bn::point InsertRoom(game_map& map, const bn::point top_left, const uint8_t option, Fog* fog){
    switch(option){
        // Rooms
        case 0:{
            return bn::point(0, 0);
        }
        case 1:{
            const uint8_t width = 7, height = 7;
            constexpr uint16_t size = width*height;

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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }
        
            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(1, 1);
        }
        case 2:{
            const uint8_t width = 7, height = 14;
            constexpr uint16_t size = width*height;

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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }
        
            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(1, 2);
        }
        case 3:{
            const uint8_t width = 7, height = 14;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,31,28,28,28,31, 0,
                    34,29,22,22,22,29,34,
                    35,33, 3, 2, 3,33,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35, 1,20,20,20, 1,35,
                    35,44,44,44,44,44,35,
                    35,22,22,22,22,22,35,
                    35, 2, 2, 2, 2, 2,35,
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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }
        
            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, ((height>>1) - 1)*32 + 16));
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 3*112),
                                        (width - 1)*32, ((height>>1) - 1)*32 + 16));
            }
            
            return bn::point(1, 2);
        }
        case 4:{
            const uint8_t width = 14, height = 7;
            constexpr uint16_t size = width*height;
            
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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }
        
            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 1);
        }
        case 5:{
            const uint8_t width = 14, height = 7;
            constexpr uint16_t size = width*height;
            
            uint8_t blockArr[size] = {
                     0,31,28,28,28,28,31, 0,31,28,28,28,31, 0,
                    34,29,22,22,22,22,29,43,29,22,22,22,29,34,
                    35,33, 3, 2, 2, 3,33,43,33, 3, 2, 3,33,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    35, 1,20,20,20,20, 1,43, 1,20,20,20, 1,35,
                    37,17,11,10,10,11,17,45,17,11,10,11,17,37,
                     0,41,40,40,40,40,41, 0,41,40,40,40,41, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }
        
            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        ((width>>1) - 1)*32, (height - 1)*32 + 16));
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 3*112), -16 + (top_left.y()*224 + 1*112),
                                        ((width>>1) - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 1);
        }
        case 6:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }

            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 2);
        }
        case 7:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

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
                    BlockFactory(map, bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
                }
            }

            if(fog){
                fog->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 2);
        }
        // Corridors
        case 8:{
            const uint8_t width = 3, height = 5;
            constexpr uint16_t size = width*height;

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
            return bn::point(0, 0);
        }
        case 9:{
            const uint8_t width = 4, height = 4;
            constexpr uint16_t size = width*height;

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
            return bn::point(0, 0);
        }
        default:{
            BN_ASSERT(false, "Invalid room option.", option);
            return bn::point(0, 0);
        }
    }
}

void GenerateLevel(game_map& map, bn::random& randomizer, Fog* fog){
    map.reset();
    if(fog){ fog->reset();}

    const int width = 4, height = 3;
    bool sectors[height][width] = { {0, 0, 0, 0},
                                    {0, 0, 0, 0},
                                    {0, 0, 0, 0} };
    
    enum Room {Empty, Small, Tall1, Tall2, Wide1, Wide2, Big1, Big2, V_Corr, H_Corr};

    // Creating rooms
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if(sectors[y][x] == true){ continue;}
            
            // Valid room selection
            bn::vector<uint8_t, ROOM_COUNT> validRooms;
            bn::point top_left(x, y);

            if(x+y == 0 || (x > 0 && sectors[y][x-1]) || (y > 0 && sectors[y-1][x])){
                validRooms.push_back(Empty);
            }
            validRooms.push_back(Small);
            if(y + 1 < height){
                if(!sectors[y+1][x]){
                    validRooms.push_back(Tall1);
                    if((y - 1 > 0 && sectors[y-1][x]) || (x - 1 > 0 && (sectors[y][x-1] || sectors[y+1][x-1])) || (x + 1 < width && (sectors[y][x+1] || sectors[y+1][x+1]))){
                        validRooms.push_back(Tall2);
                    }
                }
            }
            if(x + 1 < width){
                if(!sectors[y][x+1]){
                    validRooms.push_back(Wide1);
                    if((x - 1 > 0 && sectors[y][x-1]) || (y - 1 > 0 && (sectors[y-1][x] || sectors[y-1][x+1])) || (y + 1 < height && (sectors[y+1][x] || sectors[y+1][x+1]))){
                        validRooms.push_back(Wide2);
                    }
                }
            }
            if(x + 1 < width && y + 1 < height){
                if(!sectors[y][x+1] && !sectors[y+1][x] && !sectors[y+1][x+1]){
                    validRooms.push_back(Big1);
                    validRooms.push_back(Big2);
                }
            }

            uint8_t option = validRooms[randomizer.get_int(0, validRooms.size())];
            bn::point occupied = InsertRoom(map, top_left, option, fog);

            // Sectors update
            for(int row = y; row < y + occupied.y(); row++){
                for(int column = x; column < x + occupied.x(); column++){
                    sectors[row][column] = true;
                }
            }

        }
    }

    // Vertical corridors
    for(int y = 0; y < height - 1; y++){
        for(int x = 0; x < width; x++){
            
            if(sectors[y][x] == 0){ continue;}
            uint8_t Connected = map.cell((2 + x*7)*4, (6 + y*7)*4 + 1);     // Check if there is nothing between current and next room
            uint8_t nextExists =  map.cell((2 + x*7)*4, (7 + y*7)*4 + 1);   // Check if a room exists in the next sector
            if(!Connected && nextExists){ 
                InsertRoom(map, bn::point(2 + x*7, 5 + y*7), V_Corr);
            }
        }
    }
    // Horizontal corridors
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width - 1; x++){
            if(sectors[y][x] == 0){ continue;}
            uint8_t Connected = map.cell((6 + x*7)*4 + 1, (2 + y*7)*4);    // Check if there is nothing between current and next room
            uint8_t nextExists = map.cell((7 + x*7)*4 + 1, (2 + y*7)*4);   // Check if a room exists in the next sector
            if(!Connected && nextExists){ 
                InsertRoom(map, bn::point(5 + x*7, 2 + y*7), H_Corr);
                if(map.cell(22 + x*28, 20 + y*28) == 82){
                    uint8_t arr[4] = {91, 82,
                                      92, 84};
                    map.insert_map(game_map(2, 2, arr), bn::point(22 + x*28, 18 + y*28), true);
                }
                if(map.cell(29 + x*28, 20 + y*28) == 82){
                    uint8_t arr[4] = {91, 82,
                                      92, 84};
                    map.insert_map(game_map(2, 2, arr), bn::point(28 + x*28, 18 + y*28));
                }
            }
        }
    }
}

}