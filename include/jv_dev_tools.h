#ifndef JV_DEV_TOOLS
#define JV_DEV_TOOLS

#include "bn_sprite_palette_ptr.h"

#include "jv_actors.h"
#include "jv_interface.h"
#include "jv_constants.h"
#include "jv_level_maker.h"

#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_items_bg.h"

namespace jv::dev{
    
void blocks_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    
    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(9, 0);
    cursor.set_bg_priority(1);
    bn::sprite_ptr arrowDown = bn::sprite_items::cursor.create_sprite(0, 8);
    arrowDown.set_bg_priority(1);
    arrowDown.set_rotation_angle(90);
    arrowDown.set_visible(false);
    bn::sprite_ptr arrowUp = bn::sprite_items::cursor.create_sprite(-1, -8);
    arrowUp.set_bg_priority(1);
    arrowUp.set_rotation_angle(270);
    arrowUp.set_visible(false);
    
    bn::sprite_palette_ptr palette1 = bn::sprite_items::cursor.palette_item().create_palette();
    bn::sprite_palette_ptr palette2 = bn::sprite_items::character.palette_item().create_palette();

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr level_bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = level_bg.map();

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    // *** Level Generation ***
    uchar_t tiles_arr[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, tiles_arr);
    
    jv::LevelFactory(map1, 0);

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(4, 4);
    background.set_camera(cam);
    level_bg.set_camera(cam);

    // **** Number sprites ****
    bn::vector<bn::sprite_ptr, 128> sprts;
    int width = 12, height = 8;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if(bamod(x, 2) == 0){
                int num = (x + y*width)/2;
                if(num >= B_COUNT){break;}
                bn::string_view text = bn::to_string<8>(num);
                text_generator.generate(x*32, 4 + y*32, text, numbers);
            }
        }
    }
    for(bn::sprite_ptr sprite : numbers){
        sprite.set_camera(cam);
        sprts.push_back(sprite);
    }
    
    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    int current_block = 0, current_tile = 0, _x = 0, _y = 0;
    int tile_copy = 0;
    bool selected = false;

    while(true){
        // Movement
        if(!selected){
            if(bn::keypad::up_pressed() && _y > 0){
                _y--;
                cam.set_y(cam.y() - 8);
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
                current_block = _x/8 + (_y*mapSize.x()/80)*6;
                current_tile = tiles_arr[_x + _y*map1.x()];
                if(bamod(_x/4, 2) == 0){ cursor.set_palette(palette1);}
                else{ cursor.set_palette(palette2);}    
            }else if(bn::keypad::down_pressed() && _y < height*4 - 1){
                _y++;
                cam.set_y(cam.y() + 8);
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
                current_block = _x/8 + (_y*mapSize.x()/80)*6;
                current_tile = tiles_arr[_x + _y*map1.x()];
                if(bamod(_x/4, 2) == 0){ cursor.set_palette(palette1);}
                else{ cursor.set_palette(palette2);}
            }
            if(bn::keypad::left_pressed() && _x > 0){
                _x--;
                cam.set_x(cam.x() - 8);
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
                current_block = _x/8 + (_y*mapSize.x()/80)*6;
                current_tile = tiles_arr[_x + _y*map1.x()];
                if(bamod(_x/4, 2) == 0){ cursor.set_palette(palette1);}
                else{ cursor.set_palette(palette2);}
            }else if(bn::keypad::right_pressed() && _x < width*4 - 1){
                _x++;
                cam.set_x(cam.x() + 8);
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
                current_block = _x/8 + (_y*mapSize.x()/80)*6;
                current_tile = tiles_arr[_x + _y*map1.x()];
                if(bamod(_x/4, 2) == 0){ cursor.set_palette(palette1);}
                else{ cursor.set_palette(palette2);}
            }
            
            current_tile = tiles_arr[_x + _y*map1.x()];

            // Show block values in logging tool
            if(!NoLogs){
                if(bn::keypad::start_pressed()){
                    int start_x = (_x/8)*8, start_y = (_y/4)*4;
                    BN_LOG("block: ", current_block);
                    for(int y = start_y; y < start_y + 4; y++){
                        bn::string_view line = "";
                        for(int x = start_x; x < start_x + 4; x++){
                            line = line + bn::to_string<32>(tiles_arr[x + y*map1.x()]) + ((1 + x - start_x) + 4*(y - start_y) != 16 ? ", " : "");
                        }
                        BN_LOG(line);
                    }
                }
            }
        }else{  // Change tile
            current_tile = tiles_arr[_x + _y*map1.x()];
            if(bn::keypad::up_pressed()){
                tiles_arr[_x + _y*map1.x()] = current_tile + 1;
                tiles_arr[_x + 7 - 2*(_x%4) + _y*map1.x()] = current_tile + 1 + 127*(1 - 2*map1.horizontal_flip(_x + _y*map1.x()));
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
            }else if(bn::keypad::down_pressed() && current_tile - 1 >= 0){
                tiles_arr[_x + _y*map1.x()] = current_tile - 1;
                tiles_arr[_x + 7 - 2*(_x%4) + _y*map1.x()] = current_tile - 1 + 127*(1 - 2*map1.horizontal_flip(_x + _y*map1.x()));
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
            }
        }
        // Hide block index
        if(bn::keypad::select_pressed()){
            for(bn::sprite_ptr sprite : numbers){
                sprite.set_visible(!sprite.visible());
            }
        }// Select tile
        else if(bn::keypad::a_pressed() && bamod(_x/4, 2) == 0){
            selected = !selected;
            arrowDown.set_visible(!arrowDown.visible());
            arrowUp.set_visible(!arrowUp.visible());
        }else if(bn::keypad::l_pressed()){                          // Copy
            tile_copy = current_tile;
        }else if(bn::keypad::r_pressed() && bamod(_x/4, 2) == 0){   // Paste
            tiles_arr[_x + _y*map1.x()] = tile_copy;
            tiles_arr[_x + 7 - 2*(_x%4) + _y*map1.x()] = tile_copy + 127*(1 - 2*map1.horizontal_flip(_x + _y*map1.x()));
            jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
        }


        jv::resetcombo();
        bn::core::update();
    }
}

void tile_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::vector<bn::sprite_ptr, 3> tile_sprites;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr level_bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = level_bg.map();

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    // *** Level Generation ***
    uchar_t tiles_arr[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, tiles_arr);
    
    jv::LevelFactory(map1, 0);

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(4, 4);
    background.set_camera(cam);
    level_bg.set_camera(cam);

    // **** Number sprites ****
    bn::vector<bn::sprite_ptr, 128> sprts;
    int width = 12, height = 8;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if(bamod(x, 2) == 0){
                int num = (x + y*width)/2;
                if(num >= B_COUNT){break;}
                bn::string_view text = bn::to_string<8>(num);
                text_generator.generate(x*32, 4 + y*32, text, numbers);
            }
        }
    }
    for(bn::sprite_ptr sprite : numbers){
        sprite.set_camera(cam);
        sprts.push_back(sprite);
    }
    
    // ****** Other data ******
    int current_tile = 0, timer = 0;
    bool prev_toggle = false, toggle = false;
    int x_offset = -110, y_offset = -70;

    text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_sprites);

    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    while(true){
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
            tile_sprites.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_sprites);
            jv::LevelFactory(map1, 0);
            for(int y = 0; y < map1.y(); y++){
                for(int x = 0; x < map1.x(); x++){
                    if(map1.cell(x, y) == current_tile && x < width*4 && y < height*4){
                        tiles_arr[x + y*map1.x()] = 126;
                    }
                }
            }
            jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
        }else if(bn::keypad::r_pressed() && current_tile < 126){
            current_tile++;
            tile_sprites.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_sprites);
            jv::LevelFactory(map1, 0);
            for(int y = 0; y < map1.y(); y++){
                for(int x = 0; x < map1.x(); x++){
                    if(map1.cell(x, y) == current_tile && x < width*4 && y < height*4){
                        tiles_arr[x + y*map1.x()] = 126;
                    }
                }
            }
            jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
        }

        if(bamod(timer>>6, 2)){ toggle = true;}
        else{ toggle = false;}

        if(prev_toggle != toggle){
            if(toggle){
                for(int y = 0; y < map1.y(); y++){
                    for(int x = 0; x < map1.x(); x++){
                        if(map1.cell(x, y) == current_tile && x < width*4 && y < height*4){
                            tiles_arr[x + y*map1.x()] = 126;
                        }
                    }
                }
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
            }else{
                jv::LevelFactory(map1, 0);
                jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
            }
        }

        prev_toggle = toggle;
        timer++;
        jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);
        jv::resetcombo();
        bn::core::update();
    }
}

}


#endif