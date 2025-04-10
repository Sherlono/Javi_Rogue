#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "bn_memory.h"
#include "bn_sprites.h"
#include "bn_bgs.h"
#include "bn_blending_actions.h"

#include "jv_math.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_healthbar.h"
#include "jv_interface.h"
#include "jv_level_maker.h"
#include "jv_debug.h"

#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro1.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"
#include "bn_regular_bg_items_hud_item.h"

#include "bn_sprite_items_cursor.h"

namespace jv::game{
void intro_scene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    intro1_bg.set_priority(0);
    intro1_bg.set_blending_enabled(true);
    bn::blending::set_fade_alpha(0);
    
    for(int i = 0; i < 390; i++){
        if(i < 60){
            bn::blending::set_fade_alpha(bn::min(1-bn::fixed(i)/60, bn::fixed(1)));
        }else if(i >= 240 && i <= 300){
            bn::blending::set_fade_alpha(bn::max((bn::fixed(i)-240)/60, bn::fixed(0)));
        }
        bn::core::update();
        jv::resetcombo();
    }
    bn::blending::set_fade_alpha(0);
    intro1_bg.set_blending_enabled(false);
}
    
void start_scene(bn::random& randomizer, char& option){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    card.set_priority(0);
    card.set_blending_enabled(true);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    bg.set_priority(3);
    bg.set_blending_enabled(true);
    int y_offset = 50;
    bn::vector<bn::sprite_ptr, 64> v_text;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);
    text_generator.generate(-98, -16 + y_offset, "Select scene", v_text);
    text_generator.generate(-100, 0 + y_offset,  "Start game", v_text);
    text_generator.generate(-100, 8 + y_offset,  "Tile Showcase", v_text);

    for(bn::sprite_ptr s : v_text){ s.set_blending_enabled(true);}

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-20, y_offset);
    cursor.set_bg_priority(1);
    cursor.set_blending_enabled(true);
    
    for(int i = 0; (1 - bn::fixed(i)/fadespeed::MEDIUM) >= 0; i++){
        bn::blending::set_fade_alpha(bn::max(1 - bn::fixed(i)/60, bn::fixed(0)));
        bg.set_y(bg.y() + bn::fixed(0.25));
        if(bg.y() == 42){ bg.set_y(-54);}
        bn::core::update();
    }
    
    while(!bn::keypad::a_pressed()){
        if(bn::keypad::down_pressed() && option < 1){
            option++;
            cursor.set_position(-20, cursor.y() + 8);
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_position(-20, cursor.y() - 8);
        }

        jv::resetcombo();
        randomizer.update();
        bg.set_y(bg.y() + bn::fixed(0.25));
        if(bg.y() == 42){ bg.set_y(-54);}
        bn::core::update();
    }
    
    for(int i = 0; bn::fixed(i)/fadespeed::MEDIUM <= 1; i++){
        bn::blending::set_fade_alpha(bn::min(bn::fixed(i)/60, bn::fixed(1)));
        bg.set_y(bg.y() + bn::fixed(0.25));
        if(bg.y() == 42){ bg.set_y(-54);}
        bn::core::update();
    }

    v_text.clear();
    bn::core::update();
}

void game_scene(bn::random& randomizer){
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Music
    bn::music_items::cyberrid.play(0.25);    // Neat little song courtesy of the butano team

    // *** Level Background ***
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_priority(3);
    bn::regular_bg_ptr hud = bn::regular_bg_items::hud_item.create_bg(0, 0);
    hud.set_priority(0);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr level_bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = level_bg.map();
    level_bg.set_priority(2);

    // ****** Level data ******
    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t blockArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal);
    int level = 2;
    bool game_over = false;

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    background.set_camera(cam);
    level_bg.set_camera(cam);

    // ** Universal entities **
    jv::Player cat(0, 0, bn::sprite_items::character.create_sprite(0, 0), bn::sprite_items::character.tiles_item(), cam, &randomizer, &map1);
    jv::healthbar healthbar(cat.get_maxhp_ptr(), cat.get_hp_ptr());
    jv::stairs stairs(0, 0, cam);

    bn::vector<jv::NPC, 1> v_npcs;
    bn::vector<jv::Enemy, 15> v_enemies;

    // ****** Debug data ******
    bool val0 = false;
    bool val1 = false;
    bn::vector<jv::menu_option, 2> options;
    options.push_back(jv::menu_option(&val0, "Noclip"));
    options.push_back(jv::menu_option(&val1, "Invuln."));

    bn::vector<bn::sprite_ptr, 128> v_sprts;
    v_sprts.push_back(stairs._sprite);
    v_sprts.push_back(healthbar.bar_sprite());
    v_sprts.push_back(healthbar.corner_sprite());
    v_sprts.push_back(cat._sprite);

    bn::vector<bn::regular_bg_ptr, 4> v_bgs;
    v_bgs.push_back(background);
    v_bgs.push_back(hud);
    v_bgs.push_back(level_bg);
    
    while(!game_over){
        bool next_level = false;
        int gameover_delay = 0;

        // Level generation
        jv::LevelFactory(map1, level);
        level = level == 1 ? 2 : 1;

        bn::vector<bn::point, 25> v_points;
        jv::random_coords(v_points, map1, randomizer);

        // Reposition universal entities
        cam.set_position(v_points[0].x(), v_points[0].y());
        cat.set_position(v_points[0].x(), v_points[0].y());
        stairs.set_position(v_points[1].x(), v_points[1].y());

        // Populate level
        v_npcs.push_back(jv::NPC(v_points[2].x(), v_points[2].y(), bn::sprite_items::cow.create_sprite(0, 0), bn::sprite_items::cow.tiles_item(), cam));

        unsigned char min_enemies = 3, max_enemies = min_enemies + randomizer.get_int(v_enemies.max_size() - min_enemies);
        for(int i = 0; i < max_enemies; i++){
            v_enemies.push_back(jv::Enemy(v_points[3+i].x(), v_points[3+i].y(), bn::sprite_items::enemy.create_sprite(0, 0), bn::sprite_items::enemy.tiles_item(), cam, &randomizer, &map1));
        }
        int npcCount = v_npcs.size(), enemyCount = v_enemies.size();
        
        for(int i = 0; i < enemyCount; i++){ v_sprts.push_back(v_enemies[i]._sprite);}
        for(int i = 0; i < npcCount; i++){ v_sprts.push_back(v_npcs[i]._sprite);}
        
        // Initialize level background
        jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);

        // Fade in
        jv::fade(v_sprts, v_bgs, true);
        
        //BN_LOG("Stack memory: ", bn::memory::used_stack_iwram(), " Static memory: ", bn::memory::used_static_iwram());
        //BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());

        while(!next_level){
            if(cat.is_alive()){
                cat.update(val0);
                next_level = stairs.climb(cat);
            }
            
            healthbar.update();

            for(int i = 0; i < enemyCount; i++){
                v_enemies[i].update(&cat, val1);
                /*if(v_enemies[i].get_state() == State::DEAD){
                    v_sprts.erase(v_sprts.begin() + 1 + i);
                    v_enemies.erase(v_enemies.begin() + i);
                    enemyCount--;
                }*/
            }

            for(int i = 0; i < npcCount; i++){
                v_npcs[i].update(&cat);
            }
        
            jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);

            if(cat.is_alive()){
                if(bn::keypad::select_pressed()){ Debug::Start(options, v_sprts, v_bgs);}
            }else{
                if(gameover_delay == 120){
                    game_over = true;
                    break;
                }
                gameover_delay++;
            }

            jv::z_sort(v_sprts);
            //jv::Log_skipped_frames();
            jv::resetcombo();
            bn::core::update();
        }

        // Fade out
        jv::fade(v_sprts, v_bgs, false, cat.is_alive() ? fadespeed::MEDIUM : fadespeed::SLOW);

        // Reset Stuff
        cat.reset();
        v_npcs.clear();
        v_points.clear();
        v_enemies.clear();
        v_sprts.erase(v_sprts.begin() + 4, v_sprts.end());

        bn::core::update();
    }
}

void blocks_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Music
    bn::music_items::cyberrid.play(0.5);    // Neat little song courtesy of the butano team

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
    uchar_t blockArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal);
    
    jv::LevelFactory(map1, 0);
    // ************************

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(120, 80);
    background.set_camera(cam);
    level_bg.set_camera(cam);
    // ************************

    // **** Number sprites ****
    bn::vector<bn::sprite_ptr, 128> sprts;
    int width = 12, height = 10;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if(bamod(x, 2) == 0){
                int num = (x + y*width)/2;
                if(num >= B_COUNT){break;}
                bn::string_view text = bn::to_string<8>(num);
                text_generator.generate(x*32, 4 + y*32, text, numbers);
            }else{ continue;}
        }
    }
    for(bn::sprite_ptr sprite : numbers){
        sprite.set_camera(cam);
        sprts.push_back(sprite);
    }
    // ************************
    
    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    while(true){
        // Movement
        if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
            // Move if dir not obstructed
            if(bn::keypad::up_held()){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = cam.y() - 2*diagonal;
                cam.set_position(cam.x(), target_y);
            }else if(bn::keypad::down_held()){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::left_held() || bn::keypad::right_held());
                bn::fixed target_y = cam.y() + 2*diagonal;
                cam.set_position(cam.x(), target_y);
            }
            if(bn::keypad::left_held()){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = cam.x() - 2*diagonal;
                cam.set_position(target_x, cam.y());
            }else if(bn::keypad::right_held()){
                bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(bn::keypad::up_held() || bn::keypad::down_held());
                bn::fixed target_x = cam.x() + 2*diagonal;
                cam.set_position(target_x, cam.y());
            }
        }

        if(cam.x() > width*32){
            cam.set_position(width*32, cam.y());
        }else if(cam.x() < -16){
            cam.set_position(-16, cam.y());
        }
        if(cam.y() > (height - 1)*32){
            cam.set_position(cam.x(), (height - 1)*32);
        }else if(cam.y() < -16){
            cam.set_position(cam.x(), -16);
        }
        
        jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);
        if(bn::keypad::a_pressed()){
            for(bn::sprite_ptr sprite : numbers){
                sprite.set_visible(!sprite.visible());
            }
        }
        
        //jv::Log_skipped_frames();
        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif