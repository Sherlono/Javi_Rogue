#ifndef JV_DEV_TOOLS
#define JV_DEV_TOOLS

#include "bn_colors.h"
#include "bn_sprite_palette_ptr.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_tiled_bg.h"
#include "jv_interface.h"
#include "jv_blocks_data.h"

#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_items_bg.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

namespace jv::dev{
void GenerateDevLevel(game_map& map){
    map.reset();
    int width = 12, height = 10;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int index = x + y*width;
            jv::Level::BlockFactory(bn::point(x*4, y*4), index>>1, index%2);
        }
    }
}

void Log_block_data([[maybe_unused]] uint8_t* tiles_arr, [[maybe_unused]] game_map& map){
    for(int block_y = 0; block_y < 6; block_y++){
        for(int block_x = 0; block_x < 6; block_x++){
            if(block_x + block_y*6 >= BLOCK_TOTAL){ break;}

            #if LOGS_ENABLED
                int start_x = block_x*8, start_y = block_y*4;
                BN_LOG("//block ", block_x + block_y*6);
                for(int y = start_y; y < start_y + 4; y++){
                    bn::string_view line = (y == start_y) ? "{" : "";
                    for(int x = start_x; x < start_x + 4; x++){
                        line = line + bn::to_string<32>(tiles_arr[x + y*map.x()]) + ", ";
                    }
                    line = line + bn::to_string<32>((y == start_y + 3) ? "}, " : "");
                    BN_LOG(line);
                }
            #endif
        }
    }
}

void tile_grid_move(int& _x, int& _y, int const width, int const height, int const map_x, int& current_block, int& current_tile, uint8_t* tiles_arr, bn::camera_ptr& cam, tiled_bg& tb, bn::sprite_ptr& cursor){
    if(bn::keypad::up_pressed() && _y > 0){
        _y--;
        cam.set_y(cam.y() - 8);
        tb.init();
        current_block = (_x>>3) + (_y*map_x/80)*6;
        current_tile = tiles_arr[_x + _y*tb.map.x()];
    }else if(bn::keypad::down_pressed() && _y < height*4 - 1){
        _y++;
        cam.set_y(cam.y() + 8);
        tb.init();
        current_block = (_x>>3) + (_y*map_x/80)*6;
        current_tile = tiles_arr[_x + _y*tb.map.x()];
    }
    if(bn::keypad::left_pressed() && _x > 0){
        _x--;
        cam.set_x(cam.x() - 8);
        tb.init();
        current_block = (_x>>3) + (_y*map_x/80)*6;
        current_tile = tiles_arr[_x + _y*tb.map.x()];
        if(bamod(_x>>2, 2) == 0){ cursor.set_palette(bn::sprite_items::cursor.palette_item().create_palette());}
        else{ cursor.set_palette(bn::sprite_items::good_cat.palette_item().create_palette());}
    }else if(bn::keypad::right_pressed() && _x < width*4 - 1){
        _x++;
        cam.set_x(cam.x() + 8);
        tb.init();
        current_block = (_x>>3) + (_y*map_x/80)*6;
        current_tile = tiles_arr[_x + _y*tb.map.x()];
        if(bamod(_x>>2, 2) == 0){ cursor.set_palette(bn::sprite_items::cursor.palette_item().create_palette());}
        else{ cursor.set_palette(bn::sprite_items::good_cat.palette_item().create_palette());}
    }
    
}

void tile_change(int& _x, int& _y, int& current_tile, uint8_t* tiles_arr, tiled_bg& tb){
    if(bn::keypad::up_pressed()){
        current_tile = tiles_arr[_x + _y*tb.map.x()];
        tiles_arr[_x + _y*tb.map.x()] = current_tile + 1;
        tiles_arr[_x + 7 - 2*(_x%4) + _y*tb.map.x()] = current_tile + 1 + 127*(1 - 2*tb.map.horizontal_flip(_x + _y*tb.map.x()));
        tb.init();
    }else if(bn::keypad::down_pressed() && current_tile - 1 >= 0){
        current_tile = tiles_arr[_x + _y*tb.map.x()];
        tiles_arr[_x + _y*tb.map.x()] = current_tile - 1;
        tiles_arr[_x + 7 - 2*(_x%4) + _y*tb.map.x()] = current_tile - 1 + 127*(1 - 2*tb.map.horizontal_flip(_x + _y*tb.map.x()));
        tb.init();
    }
}

void blocks_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    
    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(9, 0);
    bn::sprite_ptr arrowDown = bn::sprite_items::cursor.create_sprite(0, 8);
    bn::sprite_ptr arrowUp = bn::sprite_items::cursor.create_sprite(-1, -8);

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();
    int width = 12, height = 6;

    // *** Level Generation ***
    uint8_t tiles_arr[cellCount*16];
    jv::tiled_bg Fortress(bn::regular_bg_tiles_items::fortress_tiles1, bn::bg_palette_items::fortress_palette, game_map(mapSize.x()*4, mapSize.y()*4, tiles_arr));
    
    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(4, 4);
    
    {// Configs
        Fortress.set_priority(3);
        cursor.set_bg_priority(1);
        arrowDown.set_bg_priority(1);
        arrowDown.set_rotation_angle(90);
        arrowDown.set_visible(false);
        arrowUp.set_bg_priority(1);
        arrowUp.set_rotation_angle(270);
        arrowUp.set_visible(false);

        background.set_camera(cam);
        Fortress.set_camera(cam);

        jv::Global::initialize(&cam, &Fortress.map, nullptr, nullptr, nullptr);
        jv::dev::GenerateDevLevel(Fortress.map);

        Fortress.init();

        // **** Number sprites ****
        text_generator.set_camera(cam);
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                if(bamod(x, 2) == 0){
                    int num = (x + y*width)/2;
                    if(num >= BLOCK_TOTAL){ break;}
                    bn::string_view text = bn::to_string<8>(num);
                    text_generator.generate(x*32, 4 + y*32, text, numbers);
                }
            }
        }
    }

    int current_block = 0, current_tile = 0, tile_copy = 0, _x = 0, _y = 0;
    bool selected = false;

    while(true){
        jv::Global::update();
        // Movement
        if(!selected){
            tile_grid_move(_x, _y, width, height, mapSize.x(), current_block, current_tile, tiles_arr, cam, Fortress, cursor);

            // Show block values in logging tool
            #if LOGS_ENABLED
                if(bn::keypad::start_pressed()){ Log_block_data(tiles_arr, Fortress.map);}
            #endif
        }
        // Change tile
        else{
            tile_change(_x, _y, current_tile, tiles_arr, Fortress);
        }
        
        // Hide block index
        if(bn::keypad::select_pressed()){
            for(bn::sprite_ptr sprite : numbers){
                sprite.set_visible(!sprite.visible());
            }
        }// Select tile
        else if(bn::keypad::a_pressed() && bamod(_x>>2, 2) == 0){
            selected = !selected;
            arrowDown.set_visible(!arrowDown.visible());
            arrowUp.set_visible(!arrowUp.visible());
        }else if(bn::keypad::l_pressed()){                          // Copy
            tile_copy = current_tile;
        }else if(bn::keypad::r_pressed() && bamod(_x>>2, 2) == 0){   // Paste
            tiles_arr[_x + _y*Fortress.map.x()] = tile_copy;
            tiles_arr[_x + 7 - 2*(_x%4) + _y*Fortress.map.x()] = tile_copy + 127*(1 - 2*Fortress.map.horizontal_flip(_x + _y*Fortress.map.x()));
            Fortress.init();
        }


        jv::Interface::resetcombo();
        bn::core::update();
    }
}

void tile_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::vector<bn::sprite_ptr, 1> tile_index_sprite;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    // *** Level Generation ***
    uint8_t tiles_arr[cellCount*16];
    jv::tiled_bg Fortress(bn::regular_bg_tiles_items::fortress_tiles1, bn::bg_palette_items::fortress_palette, game_map(mapSize.x()*4, mapSize.y()*4, tiles_arr));
    
    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(4, 4);

    // ****** Other data ******
    int current_tile = 0, timer = 0;
    bool prev_toggle = false, toggle = false;
    int x_offset = -110, y_offset = -70;
    int width = 12, height = 6;

    { // Configs
        Fortress.set_priority(3);
        text_generator.set_camera(cam);
        background.set_camera(cam);
        Fortress.set_camera(cam);

        // **** Number sprites ****
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                if(bamod(x, 2) == 0){
                    int num = (x + y*width)/2;
                    if(num >= BLOCK_TOTAL){ break;}
                    bn::string_view text = bn::to_string<8>(num);
                    text_generator.generate(x*32, 4 + y*32, text, numbers);
                }
            }
        }
        
        text_generator.remove_camera();
        text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);

        jv::Global::initialize(&cam, &Fortress.map, nullptr, nullptr, nullptr);
        jv::dev::GenerateDevLevel(Fortress.map);

        Fortress.init();
        bn::sprite_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
        bn::bg_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
    }
    
    while(true){
        jv::Global::update();

        if(bn::keypad::up_held()){
            bn::fixed target_y = cam.y() - 2;
            cam.set_position(cam.x(), target_y);
        }else if(bn::keypad::down_held()){
            bn::fixed target_y = cam.y() + 2;
            cam.set_position(cam.x(), target_y);
        }
        if(bn::keypad::left_held()){
            bn::fixed target_x = cam.x() - 2;
            cam.set_position(target_x, cam.y());
        }else if(bn::keypad::right_held()){
            bn::fixed target_x = cam.x() + 2;
            cam.set_position(target_x, cam.y());
        }

        if(bn::keypad::select_pressed()){
            for(bn::sprite_ptr sprite : numbers){
                sprite.set_visible(!sprite.visible());
            }
        }else if(bn::keypad::l_pressed() && current_tile > 0){
            current_tile--;
            tile_index_sprite.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);
            jv::dev::GenerateDevLevel(Fortress.map);
            for(int y = 0; y < Fortress.map.y(); y++){
                for(int x = 0; x < Fortress.map.x(); x++){
                    if(Fortress.map.cell(x, y) == current_tile && x < width*4 && y < height*4){
                        tiles_arr[x + y*Fortress.map.x()] = 126;
                    }
                }
            }
            Fortress.init();
        }else if(bn::keypad::r_pressed() && current_tile < 126){
            current_tile++;
            tile_index_sprite.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);
            jv::dev::GenerateDevLevel(Fortress.map);
            for(int y = 0; y < Fortress.map.y(); y++){
                for(int x = 0; x < Fortress.map.x(); x++){
                    if(Fortress.map.cell(x, y) == current_tile && x < width*4 && y < height*4){
                        tiles_arr[x + y*Fortress.map.x()] = 126;
                    }
                }
            }
            Fortress.init();
        }

        if(bamod(timer>>6, 2)){ toggle = true;}
        else{ toggle = false;}

        if(prev_toggle != toggle){
            if(toggle){
                for(int y = 0; y < Fortress.map.y(); y++){
                    for(int x = 0; x < Fortress.map.x(); x++){
                        if(Fortress.map.cell(x, y) == current_tile && x < width*4 && y < height*4){
                            tiles_arr[x + y*Fortress.map.x()] = 126;
                        }
                    }
                }
                Fortress.init();
            }else{
                jv::dev::GenerateDevLevel(Fortress.map);
                Fortress.init();
            }
        }

        prev_toggle = toggle;
        timer++;
        
        Fortress.update();
        jv::Interface::resetcombo();
        bn::core::update();
    }
}

}

#endif