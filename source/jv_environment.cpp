#include "jv_environment.h"
#include "bn_log.h"

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

bn::point InsertRoom(game_map& map, const bn::point top_left, const uint8_t option, Fog* fog_ptr){
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
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
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
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 2*112),
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
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, ((height>>1) - 1)*32 + 16));
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 3*112),
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
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 1*112),
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
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(0 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        ((width>>1) )*32, (height - 1)*32 + 16));
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 3*112), -16 + (top_left.y()*224 + 1*112),
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

            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
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

            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
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

void GenerateLevel(game_map& map, Zone& zone, bn::random& randomizer, Fog* fog_ptr){
    map.reset();
    zone.reset();
    if(fog_ptr){ fog_ptr->reset();}

    enum Room {Empty, Small, Tall1, Tall2, Wide1, Wide2, Big1, Big2, V_Corr, H_Corr};

    bn::vector<uint8_t, ROOM_COUNT> validRooms;
    bn::point top_left(0, 0);
    
    // Creating rooms
    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width; x++){
            if(zone.cell(x, y) == true){ continue;}
            
            // Valid room selection
            validRooms.clear();
            
            validRooms.push_back(Small);
            if(y + 1 < zone._height){
                validRooms.push_back(Tall1);
                if((zone.cell(x, y-1) || zone.cell(x-1, y) || zone.cell(x+1, y)) && (zone.cell(x-1, y+1) || zone.cell(x+1, y+1))){
                    validRooms.push_back(Tall2);
                }
            }
            if(x+y == 0 || (x + 1 < zone._width && !zone.cell(x+1, y))){
                validRooms.push_back(Wide1);
                if((zone.cell(x-1, y) || zone.cell(x, y-1)) && (zone.cell(x+1, y-1) || zone.cell(x+1, y+1))){
                    validRooms.push_back(Wide2);
                }
            }
            if(x+y == 0 || ((y + 1 < zone._height && x + 1 < zone._width) && !zone.cell(x+1, y) && !zone.cell(x+1, y+1))){
                validRooms.push_back(Big1);
                validRooms.push_back(Big2);
            }

            uint8_t selectedRoom = validRooms[randomizer.get_int(0, validRooms.size())];
            top_left.set_x(x);
            top_left.set_y(y);

            bn::point occupied = InsertRoom(map, top_left, selectedRoom, fog_ptr);

            // Sectors update
            for(int row = y; row < y + occupied.y(); row++){
                for(int column = x; column < x + occupied.x(); column++){
                    zone.data[column + (row*zone._width)] = true;
                }
            }

        }
    }

    // Vertical corridors
    for(int y = 0; y < zone._height - 1; y++){
        for(int x = 0; x < zone._width; x++){
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !map.cell((2 + x*7)*4, (7 + y*7)*4 + 1) || map.cell((2 + x*7)*4, (6 + y*7)*4 + 1)){ continue;}
            InsertRoom(map, bn::point(2 + x*7, 5 + y*7), V_Corr);
        }
    }
    // Horizontal corridors
    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width - 1; x++){
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !map.cell((7 + x*7)*4 + 1, (2 + y*7)*4) || map.cell((6 + x*7)*4 + 1, (2 + y*7)*4)){ continue;}
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