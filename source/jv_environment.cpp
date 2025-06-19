#include "jv_environment.h"

game_map::~game_map(){ bn::memory::clear(size(), _blocks[0]);}
game_map::game_map(uint8_t x, uint8_t y, uint8_t* blocks): width(x), height(y), _blocks(blocks){}

// Getters
[[nodiscard]] uint8_t game_map::x() const {return width;}
[[nodiscard]] uint8_t game_map::y() const {return height;}
[[nodiscard]] int game_map::size() const {return width*height;}
[[nodiscard]] bool game_map::horizontal_flip(int index) const {return _blocks[index] >= 127;}
[[nodiscard]] uint8_t game_map::cell(int x, int y) const {
    uint8_t val = _blocks[x + y*width];
    return val - 127*(val >= 127);
}

uint8_t game_map::operator[](int index){
    BN_ASSERT(index >= 0, "Invalid index", index);
    uint8_t val = _blocks[index];
    return val - 127*(val >= 127);
}

// Setters
void game_map::set_horizontal_flip(int index, bool f){
    if(f == true){
        if(horizontal_flip(index) == false){
            _blocks[index] += 127;
        }
    }else if(horizontal_flip(index) == true){
        _blocks[index] -= 127;
    }
}

void game_map::insert_map(const game_map room, const bn::point top_left, const bool fliped){
    int y_begin = top_left.y()  ,   x_begin = top_left.x();
    int aux_y = y_begin + room.height   ,   aux_x = x_begin + room.width;
    int y_end = aux_y * (aux_y < height) + height * (aux_y >= height)   ,   x_end = aux_x * (aux_x < width) + width * (aux_x >= width);

    for(int y = y_begin; y < y_end; y++){
        if(!fliped){
            for(int x = x_begin; x < x_end; x++){
                int map_index = x + y * this->width;
                int room_index = (x - x_begin) + (y - y_begin) * room.width;
                if(!room._blocks[room_index]){ continue;}
                this->_blocks[map_index] =  room._blocks[room_index];
                this->set_horizontal_flip(map_index, room.horizontal_flip(room_index));
            }
        }else{
            for(int x = x_end - 1; x >= x_begin; x--){
                int map_index = x + y * this->width;
                int room_index = -(x + 1 - x_end) + (y - y_begin) * room.width;
                if(!room._blocks[room_index]){ continue;}
                this->_blocks[map_index] = room._blocks[room_index];
                this->set_horizontal_flip(map_index, !room.horizontal_flip(room_index));
            }
        }
    }
}

void game_map::reset(){
    bn::memory::clear(size(), _blocks[0]);
}

namespace jv{
enum RoomTag {Empty, Small, Tall1, Tall2, Wide1, Wide2, Big1, Big2, V_Corr, H_Corr};

void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip){
    Common::Map().insert_map(game_map(4, 4, (uint8_t*)blocks::data[option < BLOCK_COUNT ? option : 0]), top_left, blockFlip);
}

bn::point InsertRoom(const bn::point top_left, const uint8_t option, Fog* fog_ptr){
    switch(option){
        // Rooms
        case Empty:{
            return bn::point(0, 0);
        }
        case Small:{
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x()*7)*4 - 2, (y + top_left.y()*7)*4 - 2), blockArr[index], flipArr[index]);
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
                    BlockFactory(bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
                }
            }
            return bn::point(0, 0);
        }
        case H_Corr:{
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
                    BlockFactory(bn::point((x + top_left.x())*4 - 2, (y + top_left.y())*4 - 2), blockArr[index], flipArr[index]);
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

void GenerateLevel(Zone& zone, Fog* fog_ptr){
    Common::Map().reset();
    zone.reset();
    if(fog_ptr){ fog_ptr->reset();}

    bn::vector<uint8_t, ROOM_COUNT> validRooms;
    bn::point top_left(0, 0);
    uint8_t emptyCount = 0;
    
    // Creating rooms
    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width; x++){
            if(zone.cell(x, y) == true){ continue;}
            
            // Valid room selection
            validRooms.clear();
            
            if(emptyCount < 2){
                bool Margin = !(x > 0 && x + 1 < zone._width && y > 0 && y + 1 < zone._height);
                bool Corners = zone.cell(x+1, y+1) && zone.cell(x+1, y-1) && zone.cell(x-1, y+1) && zone.cell(x-1, y-1);

                if(emptyCount == 0 || Margin || (!Margin && Corners)){
                    validRooms.push_back(Empty);
                }
            }

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
            /*if(x+y == 0 || ((y + 1 < zone._height && x + 1 < zone._width) && !zone.cell(x+1, y) && !zone.cell(x+1, y+1))){
                validRooms.push_back(Big1);
                validRooms.push_back(Big2);
            }*/

            uint8_t selectedRoom = validRooms[Common::Random().get_int(0, validRooms.size())];
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
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !Common::Map().cell((2 + x*7)*4, (7 + y*7)*4 + 1) || Common::Map().cell((2 + x*7)*4, (6 + y*7)*4 + 1)){ continue;}
            InsertRoom(bn::point(2 + x*7, 5 + y*7), V_Corr);
        }
    }
    // Horizontal corridors
    for(int y = 0; y < zone._height; y++){
        for(int x = 0; x < zone._width - 1; x++){
            // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
            if(!zone.cell(x, y) || !Common::Map().cell((7 + x*7)*4 + 1, (2 + y*7)*4) || Common::Map().cell((6 + x*7)*4 + 1, (2 + y*7)*4)){ continue;}
            InsertRoom(bn::point(5 + x*7, 2 + y*7), H_Corr);

            if(Common::Map().cell(22 + x*28, 20 + y*28) == 82){
                uint8_t arr[4] = {91, 82,
                                    92, 84};
                Common::Map().insert_map(game_map(2, 2, arr), bn::point(22 + x*28, 18 + y*28), true);
            }
            if(Common::Map().cell(29 + x*28, 20 + y*28) == 82){
                uint8_t arr[4] = {91, 82,
                                    92, 84};
                Common::Map().insert_map(game_map(2, 2, arr), bn::point(28 + x*28, 18 + y*28));
            }
        }
    }
}

}