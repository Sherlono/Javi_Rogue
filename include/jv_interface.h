#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_log.h"
#include "bn_string.h"
#include "bn_array.h"

#include "jv_math.h"
#include "jv_environment.h"

namespace jv{
void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){
        bn::core::reset();
    }
}

void Log_Skipped_Frames(){
    int skipped = bn::core::last_missed_frames();
    if(skipped != 0){BN_LOG("Frames skipped: ", skipped);}
}

struct game_map{
    game_map(cuchar_t x, cuchar_t y, uchar_t* blocks, bool* flips):width(x), height(y), _blocks(blocks), _flips(flips){}

    // Methods
    [[nodiscard]] uchar_t x(){return width;}
    [[nodiscard]] uchar_t y(){return height;}
    [[nodiscard]] int size() const {return width * height;}

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

bn::point random_start(bn::random& randomizer, game_map& map){
    bn::point* blocked = new bn::point[map.size()];
    int blockCount = 0;

    for(int y = 0; y < map.y(); y++){
        for(int x = 0; x < map.x(); x++){
            int index = x + y*map.x();
            if(map._blocks[index]){
                blocked[blockCount] = bn::point(x * 32 + 16, y * 32 + 16);
                blockCount++;
            }
        }
    }

    int block = randomizer.get_int(0, blockCount);
    bn::point out(blocked[block].x(), blocked[block].y());
    delete[] blocked;
    return out;
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

namespace LevelMaker{
static int prev_x, prev_y;

// Init must be called Once
void init(game_map& map, bn::camera_ptr& cam, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    // Defining the MAP ARRAY bounds to redraw the map
    int current_x = (cam.x().integer() + 56)/16  ,   current_y = (cam.y().integer() + 56 - 8)/16;
    prev_x = current_x;
    prev_y = current_y;

    BN_LOG("Start x: ", current_x, " Start y: ", current_y);
    
    // Redraw map bounds
    int aux_x = (current_x - 2)/2, aux_y = current_y/2;

    for(int y = aux_y; y < 8 + aux_y; y++){
        for(int x = aux_x; x < 8 + aux_x; x++){
            int xmod = bamod(x, 8), ymod = bamod(y, 8);
            bool not_oob = (x > 0 && y > 0 && x - 4 < map.x() && y - 4 < map.y() && cam.x() + x * 32 > 120 + 32 && cam.y() + y * 32 > 80 + 32);
            //bool not_oob = 1;
            
            int block_index = (x - 4 + ((y - 4) * map.x())) * not_oob;
            bn::point grid_pos(xmod, ymod);
            FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr);
        }
    }
    
    bg_map.reload_cells_ref();
}

// Update must be run every frame
void update(game_map& map, bn::camera_ptr& cam, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map, int val1, int val2, int val3, int val4){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer() + 56)/16  ,   current_y = (cam.y().integer() + 56)/16;
    
    // Redraw map bounds
    if(current_x != prev_x || current_y != prev_y){
        int aux_x = current_x/2, aux_y = current_y/2;

        for(int y = aux_y; y < 8 + aux_y; y++){
            for(int x = aux_x; x < 8 + aux_x; x++){
                int xmod = bamod(x, 8), ymod = bamod(y, 8);
                uchar_t side;

                // Horizontal
                if(current_x > prev_x && xmod == bamod(aux_x + 6, 8)){  // If moved right and currently on last column
                    //bool not_oob = (x - 4 < map.x() && y - 4 < map.y() && x  > 0 && y - 4 > 0);
                    bool not_oob = 1;
                    bn::point grid_pos(xmod, ymod);

                    if(not_oob){
                        uchar_t crop = 2 - 1*((current_x & 1) == 0);
                        int block_index = x - 4 + (y - 4)*map.x();
                        side = jv::Side::right;

                        if(y == aux_y){BN_LOG("Right ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                        FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, crop, side);
                    }else{
                        FloorFactory(grid_pos, 0, false, bg_map_ptr);
                    }
                }
                else if(current_x < prev_x && xmod == bamod(aux_x + 5, 8)){    // If moved left
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);
                    //bool not_oob = (x - 10 > 0 && y > 0 && x - 10 < map.x() && y - 4 < map.y());
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(aux_x + 6 + 1*(crop == jv::Side::right), 8), ymod);
                    
                    if(not_oob){
                        int block_index = (x - 11 + 1*(crop == jv::Side::right) + (y - 4)*map.x()) * not_oob;
                        side = jv::Side::left;

                        if(y == aux_y){BN_LOG("Left   ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                        FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, crop, side);
                    }else{
                        FloorFactory(grid_pos, 0, false, bg_map_ptr);
                    }
                }
                
                // Vertical
                if(current_y > prev_y && ymod == bamod(aux_y + 6, 8)){
                    //bool not_oob = (x > 0 && y > 0 && x - 4 < map.x() && y - 4 < map.y() && cam.x() + x * 32 > 152 && cam.y() + y * 32 > 112);
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(x - 1, 8), ymod);

                    if(not_oob){
                        uchar_t crop = 2 - 1*((current_x & 1) == 0);
                        int block_index = (x - 1 - 4 + ((y - 4)*map.x())) * not_oob;
                        side = jv::Side::down;

                        bool aux_check = (x == aux_x + 7) && (crop == 1);

                        if(x == aux_x){BN_LOG("Down", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                        FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, 1*aux_check, 2*aux_check);
                    }else{
                        FloorFactory(grid_pos, 0, false, bg_map_ptr);
                    }
                }
                else if(current_y < prev_y && ymod == bamod(aux_y + 6, 8)){
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);
                    //bool not_oob = (x - 5  + 1*(crop == jv::Side::right) > 0 && y - 4 + val2 > 0);
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(x + val3 + 1*(crop == jv::Side::right), 8), bamod(y + val4, 8));

                    if(not_oob){
                        int block_index = (x - 6 + 1*(crop == jv::Side::right) + ((y - 4 + val2) * map.x())) * not_oob;
                        side = jv::Side::up;

                        bool aux_check = (x == aux_x + val1) && (crop == 1);
                        
                        if(x == aux_x){BN_LOG("Up     ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                        FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, 1*aux_check, 2*aux_check);
                    }else{
                        FloorFactory(grid_pos, 0, false, bg_map_ptr);
                    }
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