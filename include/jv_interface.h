#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_log.h"
#include "bn_string.h"
#include "bn_array.h"

#include "jv_math.h"
#include "jv_environment.h"

namespace jv{
inline void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){ bn::core::reset();}
}

void Log_skipped_frames(){
    int skipped = bn::core::last_missed_frames();
    if(skipped != 0){BN_LOG("Frames skipped: ", skipped);}
}

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
    enum {isInt, isFloat, isBool};
    menu_option(int* d, bn::string_view t):_text(t),  _i(d), _type(isInt){}
    menu_option(bn::fixed* d, bn::string_view t): _text(t), _f(d), _type(isFloat){}
    menu_option(bool* d, bn::string_view t): _text(t), _b(d), _type(isBool){}

    void increase(){
        switch(_type){
            case isInt:
                *_i = *_i + 1;
                break;
            case isFloat:
                *_f = *_f + 0.1f;
                break;
            case isBool:
                *_b = !*_b;
                break;
            default:
                break;
        }
    }
    void decrease(){
        switch(_type){
            case isInt:
                *_i = *_i - 1;
                break;
            case isFloat:
                *_f = *_f - 0.1f;
                break;
            case isBool:
                *_b = !*_b;
                break;
            default:
                break;
        }
    }
    void print(int x, int y, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator){
        switch(_type){
            case isInt:
                text_generator.generate(x, y, bn::to_string<16>(*_i), v_text);
                break;
            case isFloat:
                text_generator.generate(x, y, bn::to_string<16>(*_f), v_text);
                break;
            case isBool:
                text_generator.generate(x, y, bn::to_string<16>(*_b), v_text);
                break;
            default:
                break;
        }
    }
    
    bn::string_view _text;
    union{ int* _i; bn::fixed* _f; bool* _b;};
    char _type;
};

void DebugUpdate(auto& options, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator, const int index, const bool increase){
    if(increase){
        options[index].increase();
    }else{
        options[index].decrease();
    }
    v_text.clear();
    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
        options[i].print(-50, -70 + 9*i, v_text, text_generator);
    }
}

namespace LevelMaker{
static int prev_x, prev_y;

// Init must be called Once
void init(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    // Defining the MAP ARRAY bounds to redraw the map
    int current_x = (cam.x().integer() + 56)/16  ,   current_y = (cam.y().integer() + 48)/16;
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
void update(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map, auto& opt){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer() + 56)/16  ,   current_y = (cam.y().integer() + 48)/16;
    
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
                else if(current_y < prev_y && ymod == bamod(aux_y + *opt[2]._i, 8)){
                    uchar_t crop = 2 - 1*((current_x & 1) == 0);
                    //bool not_oob = (x - 5  + 1*(crop == jv::Side::right) > 0 && y - 4 + *opt[1]._i > 0);
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(x + *opt[1]._i + 1, 8), bamod(y + 1, 8));

                    if(not_oob){
                        int block_index = (x - 4 + *opt[3]._i + *opt[1]._i + 1 + ((y - 11) * map.x())) * not_oob;
                        side = jv::Side::up;

                        bool aux_check = (x == aux_x + *opt[0]._i) && (crop == 1);
                        crop = 1*aux_check;
                        side = 2*aux_check;
                        
                        //if(x == aux_x){BN_LOG("Up     ", "| idx: ", block_index, "| x: ", x, "| y: ", y, "| crop: ", crop, "| side: ", side);}
                        BN_LOG("Up     ", "| x: ", x, "| aux_x: ", aux_x, "| y: ", y, "| crop: ", crop, "| side: ", side);
                        //FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr, crop, side);
                        FloorFactory(grid_pos, map._blocks[block_index], map._flips[block_index], bg_map_ptr);
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