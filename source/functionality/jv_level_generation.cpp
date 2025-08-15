#include "jv_level_generation.h"

#include "bn_assert.h"

#include "jv_global.h"
#include "jv_fog.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_interface.h"
#include "jv_map_classes.h"
#include "jv_blocks_data.h"

namespace jv::Level{
enum RoomTag {Empty, Small, Tall1, Tall2, Wide1, Wide2, Big1, Big2, V_Corr, H_Corr};

using rooms_type = bn::vector<uint8_t, ROOM_PREFAB_COUNT>;

[[nodiscard]] bn::point pop_point(bn::point* points, int& size, const int index){
    BN_ASSERT(size > 0, "Invalid size: ", size);
    BN_ASSERT(index < size, "Invalid index: ", index);
    bn::point out = bn::point(points[index].x(), points[index].y());

    points[index] = points[size-1];
    size = size - 1;
    return out;
}

void random_coords(bn::ivector<bn::point>& points_out){
    bn::point* valid_points = nullptr;
    int pointCount = 0;

    int current_size = 0;
    int width = (Global::Map().width() - 2)>>2, height = (Global::Map().height() - 3)>>2;
    int tileIndex[2][2] = {{0, 0}, {0, 0}};

    // Finding coordinates with floor in them
    for(int y = 2; y < height; y++){
        for(int x = 1; x < width; x++){
            bool walkable_check = true;
            tileIndex[0][0] = x*4 - 1;
            tileIndex[1][0] = x*4 + 2;
            tileIndex[0][1] = y*4 - 1;
            tileIndex[1][1] = y*4 + 2;

            for(int i = 0; i < 4; i++){
                uint8_t value = Global::Map().cell(tileIndex[i%2][0], tileIndex[i/2][1]);
                walkable_check = walkable_check && (value > 0 && value < WTILES_COUNT);
            }
            
            if(walkable_check){
                // Simple dinamically growing array code. Double size when capped
                pointCount++;
                if(valid_points == nullptr){
                    valid_points = new bn::point[1];
                    valid_points[0] = bn::point(32*x, 32*y);
                    current_size = 1;
                }else if(pointCount >= current_size){
                    current_size = current_size*2;
                    bn::point* temp = new bn::point[current_size];
                    for(int i = 0; i < pointCount - 1; i++){
                        temp[i] = valid_points[i];
                    }
                    temp[pointCount - 1] = bn::point(32*x, 32*y);

                    delete[] valid_points;
                    valid_points = temp;
                }else{
                    valid_points[pointCount - 1] = bn::point(32*x, 32*y);
                }
            }
        }
    }

    // Storing random valid coordinates
    for(int i = 0; i < points_out.max_size(); i++){
        int pointIndex = Global::Random().get_int(0, pointCount);
        points_out.push_back(pop_point(valid_points, pointCount, pointIndex));
    }
    delete[] valid_points;
}

void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip){
    const int index = (option < BLOCK_TOTAL) ? option : 0;
    Global::Map().insert_data(4, 4, (uint8_t*)blocks::data[index], top_left, blockFlip);
}

bn::point InsertRoom(const bn::point top_left, const uint8_t option, iFog* fog_ptr){
    bn::point target;   // Target Map cell to insert block
    switch(option){
        // Rooms
        case Small:{
            const uint8_t width = 7, height = 7;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,18,15,15,15,18, 0,
                    19,16, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 6,22,
                     0,26,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
                }
            }
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(1, 1);
        }
        case Tall1:{
            const uint8_t width = 7, height = 14;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,18,15,15,15,18, 0,
                    19,16, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 6,22,
                     0,26,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
                }
            }
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(1, 2);
        }
        case Tall2:{
            const uint8_t width = 7, height = 14;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,18,15,15,15,18, 0,
                    19,16, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20,29,29,29,29,29,20,
                    20, 9, 9, 9, 9, 9,20,
                    20, 2, 2, 2, 2, 2,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 6,22,
                     0,26,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
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
        case Wide1:{
            const uint8_t width = 14, height = 7;
            constexpr uint16_t size = width*height;
            
            uint8_t blockArr[size] = {
                     0,18,15,15,15,15,15,15,15,15,15,15,18, 0,
                    19,16, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,22,
                     0,26,25,25,25,25,25,25,25,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
                }
            }
        
            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 1);
        }
        case Wide2:{
            const uint8_t width = 14, height = 7;
            constexpr uint16_t size = width*height;
            
            uint8_t blockArr[size] = {
                     0,18,15,15,15,15,18, 0,18,15,15,15,18, 0,
                    19,16, 9, 9, 9, 9,16,31,16, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 2, 8,28, 8, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 5, 6,30, 6, 5, 5, 5, 6,22,
                     0,26,25,25,25,25,26, 0,26,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
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
        case Big1:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,18,15,15,15,15,15,15,15,15,15,15,18, 0,
                    19,16, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,22,
                     0,26,25,25,25,25,25,25,25,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
                }
            }

            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 2);
        }
        case Big2:{
            const uint8_t width = 14, height = 14;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0,18,15,15,15,15,15,15,36,15,15,15,18, 0,
                    19,16, 9, 9, 9, 9, 9, 9,28, 9, 9, 9,16,19,
                    20, 8, 2, 2, 2, 2, 2, 2,28, 2, 2, 2, 8,20,
                    20, 1, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1,28, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1,28, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1,24, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1, 9, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1, 2, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1, 1, 1, 1, 1, 1,20,
                    20, 1, 1, 1, 1,28, 1, 1, 1, 1, 1, 1, 1,20,
                    22, 6, 5, 5, 5,28, 5, 5, 5, 5, 5, 5, 6,22,
                     0,26,25,25,25, 0,25,25,25,25,25,25,26, 0 };
            bool flipArr[size] = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
            
            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x()*7)*4 - 2);
                    target.set_y((y + top_left.y()*7)*4 - 2);
                    BlockFactory(target, blockArr[index], flipArr[index]);
                }
            }

            if(fog_ptr){
                fog_ptr->create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
            }
            
            return bn::point(2, 2);
        }
        // Corridors
        case V_Corr:{
            const uint8_t width = 3, height = 5;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                     0, 1, 0,
                    27, 1,27,
                     0, 1, 0,
                     0, 1, 0,
                     0, 1, 0,};

            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x())*4 - 2);
                    target.set_y((y + top_left.y())*4 - 2);
                    BlockFactory(target, blockArr[index], index == 5);
                }
            }
        
            uint8_t aux_blockArr[4] = {
                    81,82,
                    83,84,};
            for(int x = 0; x < 2; x++){
                target.set_x(top_left.x()*4 + 6*(x == 1));
                target.set_y((2 + top_left.y())*4 - 2);
                Global::Map().insert_data(2, 2, aux_blockArr, target, (x == 1));
            }
            break;
        }
        case H_Corr:{
            const uint8_t width = 4, height = 4;
            constexpr uint16_t size = width*height;

            uint8_t blockArr[size] = {
                    0,15,15, 0,
                    0, 9, 9, 0,
                    0, 4, 4, 0,
                    0,25,25, 0 };

            for(int y = 0; y < height; y++){
                for(int x = 0; x < width; x++){
                    int index = x + y*width;
                    target.set_x((x + top_left.x())*4 - 2);
                    target.set_y((y + top_left.y())*4 - 4);
                    BlockFactory(target, blockArr[index], false);
                }
            }
            break;
        }
        default:{
            BN_ASSERT(false, "Invalid room option.", option);
            break;
        }
    }
    return bn::point(0, 0);
}

void Generate(int const zone_x, int const zone_y, iFog* fog_ptr){
    Global::Map().reset();
    bool* zData = new bool[zone_x*zone_y];
    Zone zone(zone_x, zone_y, zData);
    if(fog_ptr){ fog_ptr->reset();}

    rooms_type validRooms;
    bn::point top_left(0, 0);
    uint8_t emptyCount = 0;
    
    // Creating rooms
    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width; x++){
            if(zone.cell(x, y) == true){ continue;}
            
            // Valid room selection
            validRooms.clear();
            
            /*if(emptyCount < 2){
                bool Margin = !(x > 0 && x + 1 < zone._width && y > 0 && y + 1 < zone._height);
                bool Corners = zone.cell(x+1, y+1) && zone.cell(x+1, y-1) && zone.cell(x-1, y+1) && zone.cell(x-1, y-1);

                if(emptyCount == 0 || Margin || (!Margin && Corners)){
                    validRooms.push_back(Empty);
                }
            }*/

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
                //validRooms.push_back(Big1);
                validRooms.push_back(Big2);
            }

            uint8_t selectedRoom = validRooms[Global::Random().get_int(0, validRooms.size())];
            if(selectedRoom == Empty){
                emptyCount++;
            }

            top_left.set_x(x);
            top_left.set_y(y);

            bn::point occupied = InsertRoom(top_left, selectedRoom, fog_ptr);

            // Sectors update
            for(int row = y; row < y + occupied.y(); row++){
                for(int column = x; column < x + occupied.x(); column++){
                    int index = column + (row*zone._width);
                    zone.data[index] = true;
                }
            }

        }
    }

    // Vertical corridors
    for(int y = 0; y < zone._height - 1; y++){
        for(int x = 0; x < zone._width; x++){
            int next_cell_x = (2 + x*7)*4, next_cell_y = (7 + y*7)*4 + 1, halfway_cell_y = (6 + y*7)*4 + 1;
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !Global::Map().cell(next_cell_x, next_cell_y) || Global::Map().cell(next_cell_x, halfway_cell_y)){ continue;}
            InsertRoom(bn::point(2 + x*7, 5 + y*7), V_Corr);
        }
    }
    
    // Horizontal corridors
    uint8_t cornerFix[4] =
            {77, 82,
            78, 84,};

    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width - 1; x++){
            int next_cell_x = (7 + x*7)*4 + 1, next_cell_y = (2 + y*7)*4, halfway_cell_x = (6 + x*7)*4 + 1;
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !Global::Map().cell(next_cell_x, next_cell_y) || Global::Map().cell(halfway_cell_x, next_cell_y)){ continue;}
            InsertRoom(bn::point(5 + x*7, 2 + y*7), H_Corr);

            if(Global::Map().cell(22 + x*28, 18 + y*28) == 82){
                Global::Map().insert_data(2, 2, cornerFix, bn::point(22 + x*28, 16 + y*28), true);
            }
            if(Global::Map().cell(29 + x*28, 18 + y*28) == 82){
                Global::Map().insert_data(2, 2, cornerFix, bn::point(28 + x*28, 16 + y*28));
            }
        }
    }
    
    jv::Interface::Log_zone_layout(zone);
    delete[] zData;
}

void Populate(jv::stairs& stairs){
    const uint8_t pointsSize = 3 + MAX_ENEMIES; // positions for Player, npc, stairs and enemies
    bn::vector<bn::point, pointsSize> v_points;
    random_coords(v_points);
    
    Global::Player().set_position(v_points[0]);
    Global::Player().reset();
    Global::Camera().set_position(Global::Player().get_hitbox().x(), Global::Player().get_hitbox().y() + 4);
    stairs.set_position(v_points[1]);

    Global::NPCs().push_back(jv::NPC(v_points[2], Global::Camera()));

    bn::vector<uint8_t, 2> enemyCountList;
    enemyCountList.push_back(Global::Random().get_int(MAX_ENEMIES));
    enemyCountList.push_back(Global::Random().get_int(MAX_ENEMIES - enemyCountList[0]));
    uint8_t enemyCount[2];
    for(int i = 0; i < 2; i++){
        int index = Global::Random().get_int(enemyCountList.size());
        enemyCount[i] = enemyCountList[index];
        enemyCountList.erase(enemyCountList.begin()+index);
    }

    for(int i = 0; i < enemyCount[0]; i++){
        Global::Enemies().push_back(new jv::BadCat(v_points[3+i], Global::Camera()));
    }
    for(int i = 0; i < enemyCount[1]; i++){
        Global::Enemies().push_back(new jv::PaleTongue(v_points[3+enemyCount[0]+i], Global::Camera()));
    }
    for(int i = 0; i < MAX_ENEMIES - enemyCount[0] - enemyCount[1]; i++){
        Global::Enemies().push_back(new jv::PaleFinger(v_points[3+enemyCount[0]+enemyCount[1]+i], Global::Camera()));
    }
    
    jv::Global::update();
}

}