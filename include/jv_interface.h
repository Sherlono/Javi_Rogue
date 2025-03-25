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
    int current_x = (cam.x().integer() + 56)/8  ,   current_y = (cam.y().integer() + 48)/8;

    BN_LOG("Start x: ", current_x, " Start y: ", current_y);
    
    // Redraw map bounds
    for(int y = current_y; y < current_y + 32; y++){
        for(int x = current_x; x < current_x + 32; x++){
            bool not_oob = (x - 27 > 0 && y - 26 > 0);
            int xmod = bamod(x, 32), ymod = bamod(y, 32);

            if(x > current_x + 24 || y > current_y + 24){continue;}

            bn::point grid_pos(xmod, ymod);
            if(not_oob){
                int cell_index = x - 38 + (y - 36)*map.x();
                bg_map_ptr->set_cell(grid_pos, map._blocks[cell_index], map._flips[cell_index]);
            }else{
                bg_map_ptr->set_cell(grid_pos, 0);
            }
        }
    }
    
    prev_x = current_x;
    prev_y = current_y;
    bg_map.reload_cells_ref();
}

// Update must be run every frame
void update(bn::camera_ptr& cam, game_map& map, bn::unique_ptr<bg_map>& bg_map_ptr, bn::regular_bg_map_ptr& bg_map){
    // Did the player move enough to load assets
    int current_x = (cam.x().integer() + 56)/8  ,   current_y = (cam.y().integer() + 48)/8;

    // Redraw map bounds
    if(current_x != prev_x || current_y != prev_y){
        for(int y = current_y; y < 32 + current_y; y++){
            for(int x = current_x; x < 32 + current_x; x++){
                int xmod = bamod(x, 32), ymod = bamod(y, 32);

                // Horizontal
                if(current_x > prev_x && xmod == bamod(current_x + 24, 32)){  // If moved Right
                    bool not_oob = (x - 36 < map.x() && y - 26 - 8*3 < map.y());
                    //bool not_oob = 1;
                    bn::point grid_pos(xmod, ymod);
                    if(y == current_y){BN_LOG("Right ", "| x: ", x, "| y: ", y);}

                    if(not_oob){
                        int cell_index = x - 38 + (y - 36)*map.x();
                        bg_map_ptr->set_cell(grid_pos, map._blocks[cell_index], map._flips[cell_index]);
                    }else{
                        bg_map_ptr->set_cell(grid_pos, 0);
                    }
                }
                else if(current_x < prev_x && xmod == bamod(current_x + 25, 32)){    // If moved Left
                    bool not_oob = (x - 68 > 0 && y - 26 - 8*1 > 0);
                    //bool not_oob = 1;
                    bn::point grid_pos(xmod, ymod);
                    if(y == current_y){BN_LOG("Left   ", "| x: ", x, "| y: ", y);}

                    if(not_oob){
                        int cell_index = x - 2 + (y - 37)*map.x();
                        bg_map_ptr->set_cell(grid_pos, map._blocks[cell_index], map._flips[cell_index]);
                    }else{
                        bg_map_ptr->set_cell(grid_pos, 0);
                    }
                }
                
                // Vertical
                if(current_y > prev_y && ymod == bamod(current_y + 24, 32)){    // If moved Down
                    //bool not_oob = (x > 0 && y > 0 && x - 4 < map.x() && y - 4 < map.y() && cam.x() + x * 32 > 152 && cam.y() + y * 32 > 112);
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(x - 7, 32), ymod);
                    if(x == current_x){BN_LOG("Down", "| x: ", x, "| y: ", y);}

                    if(not_oob){
                        int cell_index = x - 45 + (y - 36)*map.x();
                        bg_map_ptr->set_cell(grid_pos, map._blocks[cell_index], map._flips[cell_index]);
                    }else{
                        bg_map_ptr->set_cell(grid_pos, 0);
                    }
                }
                else if(current_y < prev_y && ymod == bamod(current_y, 32)){   // If moved Up
                    //bool not_oob = (x - 5 > 0 && y - 4 + *opt[1]._i > 0);
                    bool not_oob = 1;
                    bn::point grid_pos(bamod(x - 7, 32), ymod);
                    if(x == current_x){BN_LOG("Up     ", "| x: ", x, "| y: ", y);}

                    if(not_oob){
                        int cell_index = x - 45 + (y - 36)*map.x();
                        bg_map_ptr->set_cell(grid_pos, map._blocks[cell_index], map._flips[cell_index]);
                    }else{
                        bg_map_ptr->set_cell(grid_pos, 0);
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