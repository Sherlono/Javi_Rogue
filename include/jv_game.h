#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_vector.h"
#include "bn_memory.h"
#include "bn_colors.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"

#include "jv_fog.h"
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

#include "bn_sprite_items_cursor.h"
#include "bn_sprite_items_ball.h"

namespace jv::game{
void intro_scene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    bn::sprite_palettes::set_fade(bn::colors::black, 0);
    bn::bg_palettes::set_fade(bn::colors::black, 0);
    
    jv::fade(true);
    for(int i = 0; i < 180; i++){ bn::core::update();}
    jv::fade(false);

    bn::sprite_palettes::set_fade(bn::colors::black, 0);
    bn::bg_palettes::set_fade(bn::colors::black, 0);
}
    
void start_scene(bn::random& randomizer, char& option){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    card.set_priority(0);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    bg.set_priority(3);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 45> menu_sprts;
    char y_offset = 50;
    text_generator.set_bg_priority(0);
    text_generator.generate(-98, y_offset - 16, "Select scene", menu_sprts);
    text_generator.generate(-100, y_offset,     "Start game", menu_sprts);
    text_generator.generate(-100, y_offset + 8, "Block test", menu_sprts);
    text_generator.generate(-100, y_offset + 16,"Tile test", menu_sprts);

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-34, y_offset);
    cursor.set_bg_priority(1);
    
    bn::vector<bn::sprite_ptr, 83> explain_sprts;
    bn::string_view explain_text[3][5] = {{"", "A: Interact", "B: Attack", "SELECT: Debug menu"},
                                          {"A: Select tile", "L: Copy tile", "R: Paste tile", "SELECT: Toggle index", "START: Print to log"},
                                          {"", "L: Next highlighted tile", "R: Prev. highlighted tile", "SELECT: Toggle index"}};

    int x_offset = -16;
    y_offset = 40;
    
    for(int i = 0; i < 5; i++){
        text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
    }

    // Fade in
    bn::fixed fadeProgress = 1.0, scrollSpeed = 0.25, start_y = -48, end_y = start_y + 96;
    for(int i = 0; fadeProgress >= 0; i++){
        fadeProgress = 1 - bn::fixed(i)/fadespeed::MEDIUM;
        bn::sprite_palettes::set_fade(bn::colors::black, bn::max(fadeProgress, bn::fixed(0)));
        bn::bg_palettes::set_fade(bn::colors::black, bn::max(fadeProgress, bn::fixed(0)));
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }

    // Selecting a scene
    while(!bn::keypad::a_pressed()){
        if(bn::keypad::down_pressed() && option < 2){
            option++;
            cursor.set_y(cursor.y() + 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_y(cursor.y() - 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }

        jv::resetcombo();
        randomizer.update();
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }

    // Fade out
    for(int i = 0; fadeProgress <= 1; i++){
        fadeProgress = bn::fixed(i)/fadespeed::MEDIUM;
        bn::sprite_palettes::set_fade(bn::colors::black, bn::min(fadeProgress, bn::fixed(1)));
        bn::bg_palettes::set_fade(bn::colors::black, bn::min(fadeProgress, bn::fixed(1)));
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }
    
    bn::core::update();
}

void game_scene(bn::random& randomizer){
    // Text generator
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);

    // Music
    bn::music_items::cyberrid.play(0.1);

    // *** Level Background ***
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_priority(3);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr level_bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = level_bg.map();
    level_bg.set_priority(2);

    // ****** Level data ******
    constexpr bn::point mapSize(27, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uint8_t tiles_arr[cellCount*16 + (mapSize.y()*4*2)];
    game_map mainGameMap(mapSize.x()*4 + 2, mapSize.y()*4, tiles_arr);

    int floor = 0, gameover_delay = 0;
    bool next_level = false, game_over = false, objective = true;

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    background.set_camera(cam);
    level_bg.set_camera(cam);

    // ** Universal entities **
    jv::Player cat(bn::point(0, 0), cam, &randomizer, &mainGameMap);
    jv::healthbar healthbar(cat.get_maxhp_ptr(), cat.get_hp_ptr());
    jv::stairs stairs(0, 0, cam);

    bn::vector<jv::NPC, 1> v_npcs;
    bn::vector<jv::Enemy*, 10> v_enemies;
    bn::vector<bn::sprite_ptr, 2> txt_sprts;
    text_generator.generate(64, -70, "Floor", txt_sprts);

    jv::Fog fog(cam);
    bn::blending::set_transparency_alpha(0.8);

    // ****** Debug data ******
    bool val0 = false;
    bool val1 = false;
    bn::vector<jv::menu_option, 5> options;
    options.push_back(jv::menu_option(&val0, "Noclip"));
    options.push_back(jv::menu_option(&val1, "Invuln."));
    options.push_back(jv::menu_option(&next_level, "Next level"));

    /*bn::vector<bn::sprite_ptr, 8> v_balls;
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            if(!(x == 1 && y == 1)){
                v_balls.push_back(bn::sprite_items::ball.create_sprite(mainGameMap.x()*4*x, mainGameMap.y()*4*y));
                v_balls[v_balls.size() - 1].set_bg_priority(0);
                v_balls[v_balls.size() - 1].set_camera(cam);
            }
        }
    }*/
    
    while(!game_over){
        text_generator.generate(94, -70, bn::to_string<3>(floor), txt_sprts);
        next_level = false;
        gameover_delay = 0;

        // Level generation
        jv::GenerateLevel(mainGameMap, fog, randomizer);

        {
            const uint8_t pointsSize = 32;
            bn::vector<bn::point, pointsSize> v_points;
            jv::random_coords(v_points, mainGameMap, randomizer);
            
            cam.set_position(v_points[0]);
            cat.set_position(v_points[0]);
            fog.update(cat.position());
            stairs.set_position(v_points[1]);

            // Populate level
            v_npcs.push_back(jv::NPC(v_points[2], cam));

            uint8_t min_enemies = v_enemies.max_size()/3;
            uint8_t max_enemies = min_enemies + randomizer.get_int(v_enemies.max_size() - min_enemies);
            for(int i = 0; i < max_enemies; i++){
                v_enemies.push_back(new jv::BadCat(v_points[3+i], cam, &randomizer, &mainGameMap));
            }
        }

        // Initialize level background
        jv::LevelMaker::init(cam, mainGameMap, bg_map_ptr, bg_map);
        
        // Fade in
        jv::fade(true);
        
        jv::Log_resources();

        while(!next_level){
            objective = true;

            if(cat.alive()){
                cat.update(val0);
                next_level = stairs.climb(cat.rect(), cat.get_state());
                fog.update(cat.position());
            }

            for(int i = 0; i < v_enemies.size(); i++){
                v_enemies[i]->update(&cat, cam, val1);
                /*if(v_enemies[i].get_state() == State::DEAD){
                    v_enemies.erase(v_enemies.begin() + i);
                    enemyCount--;
                }*/
                objective = objective && !v_enemies[i]->alive();
            }
            healthbar.update();

            for(int i = 0; i < v_npcs.size(); i++){ v_npcs[i].update(cat, cam, stairs, objective);}

            if(cat.alive()){
                if(bn::keypad::select_pressed()){
                    healthbar.set_visible(false);
                    Debug::Start(options);
                    healthbar.set_visible(true);
                }
            }else{
                if(gameover_delay == 120){
                    game_over = true;
                    break;
                }
                gameover_delay++;
            }
            
            jv::LevelMaker::update(cam, mainGameMap, bg_map_ptr, bg_map);

            jv::Log_skipped_frames();
            jv::resetcombo();
            bn::core::update();
        }
        floor--;
        
        // Fade out
        jv::fade(false, cat.alive() ? fadespeed::MEDIUM : fadespeed::SLOW);

        // Reset Stuff
        stairs.set_open(false);
        cat.reset();
        fog.reset();
        v_npcs.clear();
        for(int i = 0; i < v_enemies.size(); i++){ delete v_enemies[i];}
        v_enemies.clear();
        txt_sprts.erase(txt_sprts.begin() + 1);

        bn::core::update();
    }
}

}

#endif