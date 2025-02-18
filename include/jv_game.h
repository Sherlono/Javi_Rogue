#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_interface.h"
#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{
void debug_mode(bn::vector<int*, 5>& myrefs, bn::regular_bg_ptr bg, bn::regular_bg_ptr floor, jv::Player& player){
    bg.set_visible(false);
    floor.set_visible(false);
    player.set_visible(false);

    bn::vector<bn::sprite_ptr, 128> v_text;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);

    static int index = 0;
    bn::sprite_ptr cursor = bn::sprite_items::ball.create_sprite(-25, -70 + 9*index);

    bn::vector<jv::menu_option, 5> options;
    options.push_back(jv::menu_option("Val 1", *myrefs[0]));
    options.push_back(jv::menu_option("Column", *myrefs[1]));
    options.push_back(jv::menu_option("Block", *myrefs[2]));

    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
        text_generator.generate(-50, -70 + 9*i, bn::to_string<6>(options[i].data), v_text);
    }

    bn::core::update();
    while(true){

        if(bn::keypad::down_pressed()){
            if(index < options.size()){
                index++;
                cursor.set_position(cursor.x(), cursor.y() + 9);
            }
        }else if(bn::keypad::up_pressed()){
            if(index > 0){
                index--;
                cursor.set_position(cursor.x(), cursor.y() - 9);
            }
        }
        if(bn::keypad::left_pressed()){
            options[index].data--;
            v_text.clear();
            for(int i = 0; i < options.size(); i++){
                text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
                text_generator.generate(-50, -70 + 9*i, bn::to_string<16>(options[i].data), v_text);
            }
        }else if(bn::keypad::right_pressed()){
            options[index].data++;
            v_text.clear();
            for(int i = 0; i < options.size(); i++){
                text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
                text_generator.generate(-50, -70 + 9*i, bn::to_string<16>(options[i].data), v_text);
            }
        }

        if(bn::keypad::start_pressed()){
            bg.set_visible(true);
            floor.set_visible(true);
            player.set_visible(true);
            
            for(int i = 0; i < options.size(); i++){
                BN_LOG(options[i]._text, ": ", options[i].data);
            }
            break;
        }

        jv::resetcombo();
        bn::core::update();
    }
}

void game_scene(bn::camera_ptr& cam, bn::random* random_ptr){
    // Player init
    const bn::point start_pos (-70, -80);
    cam.set_position(start_pos.x(), start_pos.y());
    jv::Player cat(start_pos.x(), start_pos.y(), random_ptr);
    
    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);

    bn::vector<bn::sprite_ptr, 64> balls;
    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 4; x++){
            bn::sprite_ptr ball = bn::sprite_items::ball.create_sprite((x*32)-96, (y*32)-96);
            ball.set_camera(cam);
            balls.push_back(ball);
        }
    }

    int prev_skipped = 0;
    int val1 = 6, val2 = -1, val3 = -1;
    
    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();
    bg.set_camera(cam);

    uchar_t blockArr[306] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 9, 3, 2, 2, 3, 9, 0, 0, 0, 9, 3, 2, 2, 3, 9, 0,
                              0, 1,20,20,20,20, 1, 0, 0, 0, 1,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20, 5, 2, 2, 2, 5,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20,13,10,10,10,13,20,20,20,20, 1, 0,
                              0,17,11,10,20,11,17, 0, 0, 0,17,11,20,10,11,17, 0,
                              0, 0, 0, 0,20, 0, 0, 0, 0, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0, 0, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20,20,20,20,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20,20,20,20,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0, 0, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 9, 3, 2,20, 3, 9, 0, 0, 0, 9, 3,20, 2, 3, 9, 0,
                              0, 1,20,20,20,20, 1, 0, 0, 0, 1,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20, 5, 2, 2, 2, 5,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20,13,10,10,10,13,20,20,20,20, 1, 0,
                              0,17,11,10,10,11,17, 0, 0, 0,17,11,10,10,11,17, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    bool flipArr[306] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                            0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    game_map map1(17, 9, blockArr, flipArr);

    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            //bool not_outside = (x < map1.x() && y < map1.y() && x > 0 && y > 0 && cam.x() + x * 32 < 120 && cam.y() + y * 32 < 80);
            bool not_outside = 1;
            int index = (x + y*map1.x()) * not_outside;
            //BN_LOG("x: ", x, " y: ", y, " index: ", index);
            FloorFactory(bn::point(x, y), map1._blocks[index], map1._flips[index], bg_map_ptr);
        }
    }
    bg_map.reload_cells_ref();

    
    while(true){
        /*badcat.update(cat);
        cow.update(cat);*/
        cat.update(cam);
        
        if(bn::keypad::start_pressed()){
            bn::vector<int*, 5> myrefs;
            myrefs.push_back(&val1);
            myrefs.push_back(&val2);
            myrefs.push_back(&val3);
            debug_mode(myrefs, background, bg, cat);
        }

        int skipped = bn::core::last_missed_frames();
        if(prev_skipped != skipped && skipped != 0){ BN_LOG("Frames skipped: ", skipped);}
        prev_skipped = skipped;

        jv::LevelMaker::update(map1, cam, bg_map_ptr, val1, val2, val3);
        bg_map.reload_cells_ref();

        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif