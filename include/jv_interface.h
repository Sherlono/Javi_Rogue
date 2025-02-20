#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_log.h"
#include "bn_string.h"
#include "bn_array.h"

#include "jv_environment.h"

namespace jv{
void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){
        bn::core::reset();
    }
}

struct menu_option{
    menu_option(bn::string_view t, int& d):_text(t), data(d){}
    bn::string_view _text;
    int& data;
};

void DebugUpdate(bn::vector<jv::menu_option, MAX_OPTIONS>& options, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator, const int index, const bool increase){
    if(increase){
        options[index].data++;
    }else{
        options[index].data--;
    }
    v_text.clear();
    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
        text_generator.generate(-50, -70 + 9*i, bn::to_string<16>(options[i].data), v_text);
    }
}

struct game_map{
    game_map(cuchar_t x, cuchar_t y, uchar_t* blocks, bool* flips):width(x), height(y), _blocks(blocks), _flips(flips){}

    // Methods
    [[nodiscard]] uchar_t x(){return width;}
    [[nodiscard]] uchar_t y(){return height;}
    [[nodiscard]] bn::point size(){return bn::point(width, height);}

    // Insert room into the main map starting by the top left corner
    void insert_room(const game_map room, const bn::point top_left){
        int y_begin = top_left.y()  ,   x_begin = top_left.x();
        int aux_y = y_begin + room.height   ,   aux_x = x_begin + room.width;
        int y_end = aux_y * (aux_y < height) + height * (aux_y >= height)   ,   x_end = aux_x * (aux_x < width) + width * (aux_x >= width);

        for(int y = y_begin; y < y_end; y++){
            for(int x = x_begin; x < x_end; x++){
                int map_index = x + y * this->width;
                int room_index = (x - x_begin) + (y - y_begin) * room.width;
                this->_blocks[map_index] =  room._blocks[room_index];
                this->_flips[map_index] =  room._flips[room_index];
            }
        }
    }
    // Members
    uchar_t width, height;
    uchar_t* _blocks;
    bool* _flips;
};

namespace LevelMaker{
// Init must be called Once
void init(game_map& map, bn::camera_ptr& cam, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    // Defining the MAP ARRAY bounds to redraw the map
    int current_x = (cam.x().integer() + 56-32)/16  ,   current_y = (cam.y().integer() + 56)/16;
    BN_LOG(" x: ", current_x, " y: ", current_y);
    
    // Redraw map bounds
    int aux_x = current_x/2, aux_y = current_y/2;

    for(int y = aux_y; y < 8 + aux_y; y++){
        for(int x = aux_x; x < 8 + aux_x; x++){
            int xmod = x % 8, ymod = y % 8;
            
            //uchar_t crop = 2 - 1*((current_x & 1) == 0);    // Checks whether to draw left or right half of the block column
            bool not_oob = (x > 0 && y > 0 && x - 4 < map.x() && y - 4 < map.y() && cam.x() + x * 32 > 120 + 32 && cam.y() + y * 32 > 80 + 32);
            //bool not_oob = 1;
            
            int block_index = (x - 4 + ((y - 4) * map.x())) * not_oob;
            bn::point grid_pos(xmod, ymod);

            //BN_LOG(" x: ", x, " y: ", y, " map.x(): ", map.x(), " map.y(): ", map.y());
            FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr);
        }
    }
    
    bg_map.reload_cells_ref();
}

// Update must be run every frame
static int prev_x = 3, prev_y = 3;
void update(game_map& map, bn::camera_ptr& cam, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map, int val1, int val2, int val3, int val4){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer() + 56)/16  ,   current_y = (cam.y().integer() + 56)/16;
    //BN_LOG(" x: ", current_x, " y: ", current_y);
    
    // Redraw map bounds
    if(current_x != prev_x || current_y != prev_y){
        int aux_x = current_x/2, aux_y = current_y/2;

        for(int y = aux_y; y < 8 + aux_y; y++){
            for(int x = aux_x; x < 8 + aux_x; x++){
                int xmod = x % 8, ymod = y % 8;
                uchar_t side;

                // Horizontal
                if(current_x > prev_x && xmod == (aux_x + 6) % 8){  // If moved right and currently on last column
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);    // Checks whether to draw left or right half of the block column
                    bool not_oob = (x - 4 < map.x() && y - 4 < map.y() && x  > 0 && y - 4 > 0);
                    
                    int block_index = (x - 4 + ((y - 4) * map.x())) * not_oob;
                    bn::point grid_pos(xmod, ymod);
                    side = jv::Side::right;

                    if(y == aux_y){ BN_LOG("Right ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                    FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, crop, side);
                }
                else if(current_x < prev_x && xmod == (aux_x + 5) % 8){    // If moved left
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);    // Checks whether to draw left or right half of the block column
                    bool not_oob = (x - 10 > 0 && y > 0 && x - 10 < map.x() && y - 4 < map.y());
                    
                    int block_index = (x - 7 - 4 + 1*(crop == jv::Side::right) + (y - 4) * map.x()) * not_oob;
                    bn::point grid_pos((aux_x + 6 + 1*(crop == jv::Side::right)) % 8, ymod);
                    side = jv::Side::left;

                    if(y == aux_y){ BN_LOG("Left   ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                    FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, crop, side);
                }
                
                // Vertical
                if(current_y > prev_y && ymod == (aux_y + 6) % 8){
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);
                    bool not_oob = (x > 0 && y > 0 && x - 4 < map.x() && y - 4 < map.y() && cam.x() + x * 32 > 152 && cam.y() + y * 32 > 112);

                    int block_index = (x - 1 - 4 + ((y - 4) * map.x())) * not_oob;
                    bn::point grid_pos((x - 1) % 8, ymod);
                    side = jv::Side::down;

                    if(x == aux_x){ BN_LOG("Down ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                    FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr);
                }
                else if(current_y < prev_y && ymod == (aux_y + val1) % 8){
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);
                    //bool not_oob = (x > 0 && y > 0 && cam.x() + x * 32 > 120 + 32 && cam.y() + y * 32 > 80 + 32);
                    bool not_oob = (x - 5 + 1*(crop == jv::Side::right) > 0 && y - 4 + val2 > 0);
                    //bool not_oob = 1;

                    int block_index = (x - 6 + 1*(crop == jv::Side::right) + ((y - 4 + val2) * map.x())) * not_oob;
                    bn::point grid_pos((x + val3 + 1*(crop == jv::Side::right)) % 8, (y + val4) % 8);
                    side = jv::Side::up;

                    if(x == aux_x){ BN_LOG("Up     ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                    FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, 0, side);
                }
            }
        }
    }
    prev_x = current_x;
    prev_y = current_y;
    
    bg_map.reload_cells_ref();
}
}
}

#endif