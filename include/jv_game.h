#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_actors.h"
#include "jv_interface.h"
#include "jv_environment.h"

#include "bn_sprite_items_ball.h"
#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{
void debug_mode(auto& options, bn::vector<bn::regular_bg_ptr, 3> bgs, bn::vector<bn::sprite_ptr, 10> sprts){
    // Hide all previous graphics
    for(int i = 0; i < sprts.size(); i++){
        if(i < bgs.size()){ bgs[i].set_visible(false);}
        sprts[i].set_visible(false);
    }

    static int index = 0;
    uchar_t hold = 0;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-20, -70 + 9*index);
    bn::vector<bn::sprite_ptr, 128> v_text;

    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
        options[i].print(-50, -70 + 9*i, v_text, text_generator);
    }

    bn::core::update();

    while(!bn::keypad::start_pressed()){
        if(bn::keypad::down_pressed()){
            if(index < options.size() - 1){
                index++;
                cursor.set_position(cursor.x(), cursor.y() + 9);
            }
        }else if(bn::keypad::up_pressed()){
            if(index > 0){
                index--;
                cursor.set_position(cursor.x(), cursor.y() - 9);
            }
        }
        
        if(bn::keypad::a_pressed()){ DebugUpdate(options, v_text, text_generator, index, true);}
        else if(bn::keypad::b_pressed()){ DebugUpdate(options, v_text, text_generator, index, false);}

        if(bn::keypad::a_held()){
            hold++;
            if(hold > 6){
                DebugUpdate(options, v_text, text_generator, index, true);
                hold = 0;
            }
        }else if(bn::keypad::b_held()){
            hold++;
            if(hold > 6){
                DebugUpdate(options, v_text, text_generator, index, false);
                hold = 0;
            }
        }

        if(bn::keypad::a_released() || bn::keypad::b_released()){hold = 0;}

        jv::resetcombo();
        bn::core::update();
    }
    
    // Unhide all previous graphics
    for(int i = 0; i < sprts.size(); i++){
        if(i < bgs.size()){ bgs[i].set_visible(true);}
        sprts[i].set_visible(true);
    }

    // Print debug values
    for(int i = 0; i < options.size(); i++){
        BN_LOG(options[i]._text, ": ", *options[i]._i);
    }
}

void start_scene(bn::random& randomizer){
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 64> v_text;
    text_generator.generate(-110, 0, "Press any button to start.", v_text);
    text_generator.set_bg_priority(0);
    
    while(!bn::keypad::any_pressed()){
        randomizer.update();
        bn::core::update();
    }
    v_text.clear();
    bn::core::update();
}

void game_scene(bn::random& randomizer){
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Music
    bn::music_items::cyberrid.play(0.5);    // Neat little song courtesy of the butano team

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();
    bg.set_camera(cam);

    uchar_t blockArr[306] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 9, 3, 2, 2, 3, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 1,20,20,20,20, 1, 0, 0, 0, 9, 3, 2, 3, 9, 0, 0,
                              0, 1,20,20,20,20, 5, 2, 2, 2, 5,20,20,20, 1, 0, 0,
                              0, 1,20,20,20,20,13,10,20,10,13,20,20,20, 1, 0, 0,
                              0,17,11,10,20,11,17, 0,20, 0,17,11,20,11,17, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20,20,20,20,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20,20,20,20,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 9, 3,20, 3, 9, 0,20, 0, 0, 0, 0,
                              0, 0, 0, 0,20, 0, 1,20,20,20, 1, 0,20, 0, 0, 0, 0,
                              0, 9, 3, 2,20, 2, 5,20,20,20, 5, 2,20, 2, 3, 9, 0,
                              0, 1,20,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0,
                              0, 1,20,20,20,20,13,10,10,10,13,20,20,20,20, 1, 0,
                              0,17,11,10,10,11,17, 0, 0, 0,17,11,10,10,11,17, 0};

    bool flipArr[306] =     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
                              0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
                              0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
                              0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0};

    game_map map1(17, 18, blockArr, flipArr);

    // Selecting random initial position
    bn::point start_pos = random_start(randomizer, map1);
    cam.set_position(start_pos.x(), start_pos.y());

    // Characters initialization
    jv::Player cat(start_pos.x(), start_pos.y(), randomizer, bg_map_ptr);
    start_pos = random_start(randomizer, map1);
    jv::NPC cow(start_pos.x(), start_pos.y());
    cow.set_camera(cam);
    start_pos = random_start(randomizer, map1);
    jv::Enemy badcat(start_pos.x(), start_pos.y(), randomizer);
    badcat.set_camera(cam);

    // Debug data
    int val1 = 8, val2 = -2, val3 = 6, val4 = 0;
    bool val5 = true;
    bn::fixed val6 = 23.2;
    bn::vector<jv::menu_option, 6> options;
    options.push_back(jv::menu_option(&val1, "Opt[0]"));
    options.push_back(jv::menu_option(&val2, "Opt[1]"));
    options.push_back(jv::menu_option(&val3, "Opt[2]"));
    options.push_back(jv::menu_option(&val4, "Opt[3]"));
    options.push_back(jv::menu_option(&val5, "Opt[4]"));
    options.push_back(jv::menu_option(&val6, "Opt[5]"));
    
    
    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    while(true){
        cat.update(cam);
        cow.update(cat);
        badcat.update(cat);
        
        jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map, options);

        if(bn::keypad::start_pressed()){
            bn::vector<bn::regular_bg_ptr, 3> bgs;
            bgs.push_back(background);
            bgs.push_back(bg);
            bn::vector<bn::sprite_ptr, 10> sprts;
            sprts.push_back(cat._sprite);
            sprts.push_back(badcat._sprite);
            sprts.push_back(cow._sprite);
            jv::game::debug_mode(options, bgs, sprts);
        }

        jv::Log_skipped_frames();
        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif