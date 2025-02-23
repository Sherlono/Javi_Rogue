#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_interface.h"
#include "bn_sprite_items_ball.h"
#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{
void debug_mode(auto& optionRefs, bn::sprite_text_generator& text_generator, bn::regular_bg_ptr bg, bn::regular_bg_ptr floor, jv::Player& player){
    bg.set_visible(false);
    floor.set_visible(false);
    player.set_visible(false);

    bn::vector<bn::sprite_ptr, 128> v_text;
    static int index = 0;
    uchar_t hold = 0;

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-25, -70 + 9*index);

    bn::vector<jv::menu_option, MAX_OPTIONS> options;
    options.push_back(jv::menu_option("Val 1", *optionRefs[0]));
    options.push_back(jv::menu_option("Val 2", *optionRefs[1]));
    options.push_back(jv::menu_option("Val 3", *optionRefs[2]));
    options.push_back(jv::menu_option("Val 4", *optionRefs[3]));

    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i]._text, v_text);
        text_generator.generate(-50, -70 + 9*i, bn::to_string<6>(options[i].data), v_text);
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
    
    bg.set_visible(true);
    floor.set_visible(true);
    player.set_visible(true);
    
    for(int i = 0; i < options.size(); i++){
        BN_LOG(options[i]._text, ": ", options[i].data);
    }
}

void start_scene(bn::sprite_text_generator& text_generator, bn::random& randomizer){
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

void game_scene(bn::camera_ptr& cam, bn::sprite_text_generator& text_generator, bn::random& randomizer){
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

    // NPC init
    jv::Player cat(start_pos.x(), start_pos.y(), randomizer, bg_map_ptr);
    start_pos = random_start(randomizer, map1);
    jv::NPC cow(start_pos.x(), start_pos.y());
    cow.set_camera(cam);

   
    int val1 = 7, val2 = -7, val3 = -2, val4 = 1;
    
    
    jv::LevelMaker::init(map1, cam, bg_map_ptr, bg_map);
    
    while(true){
        //badcat.update(cat);
        cat.update(cam);
        cow.update(cat);
        
        if(bn::keypad::start_pressed()){
            bn::vector<int*, 5> optionRefs;
            optionRefs.push_back(&val1);
            optionRefs.push_back(&val2);
            optionRefs.push_back(&val3);
            optionRefs.push_back(&val4);
            debug_mode(optionRefs, text_generator, background, bg, cat);
        }


        jv::LevelMaker::update(map1, cam, bg_map_ptr, bg_map, val1, val2, val3, val4);

        //jv::Log_Skipped_Frames();
        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif