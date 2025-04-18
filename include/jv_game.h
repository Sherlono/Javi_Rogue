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
    intro1_bg.set_blending_enabled(true);
    bn::blending::set_fade_alpha(0);
    
    jv::fade(true);
    for(int i = 0; i < 180; i++){ bn::core::update();}
    jv::fade(false);

    bn::blending::set_fade_alpha(0);
}
    
void start_scene(bn::random& randomizer, char& option){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    card.set_priority(0);
    card.set_blending_enabled(true);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    bg.set_priority(3);
    bg.set_blending_enabled(true);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 45> menu_sprts;
    int y_offset = 50;
    text_generator.set_bg_priority(0);
    text_generator.generate(-98, y_offset - 16, "Select scene", menu_sprts);
    text_generator.generate(-100, y_offset,     "Start game", menu_sprts);
    text_generator.generate(-100, y_offset + 8, "Block test", menu_sprts);
    text_generator.generate(-100, y_offset + 16,"Tile test", menu_sprts);

    for(bn::sprite_ptr s : menu_sprts){ s.set_blending_enabled(true);}

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-34, y_offset);
    cursor.set_bg_priority(1);
    cursor.set_blending_enabled(true);
    
    bn::vector<bn::sprite_ptr, 83> explain_sprts;
    bn::string_view explain_text[3][5] = {{"", "A: Interact", "B: Attack", "SELECT: Debug menu"},
                                          {"A: Select tile", "L: Copy tile", "R: Paste tile", "SELECT: Toggle index", "START: Print to log"},
                                          {"", "L: Next highlighted tile", "R: Prev. highlighted tile", "SELECT: Toggle index"}};

    int x_offset = -16;
    y_offset = 40;
    
    for(int i = 0; i < 5; i++){
        text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
    }
    for(bn::sprite_ptr s : explain_sprts){ s.set_blending_enabled(true);}

    // Fade in
    bn::fixed fadeProgress = 1.0, scrollSpeed = 0.25, start_y = -48, end_y = start_y + 96;
    for(int i = 0; fadeProgress >= 0; i++){
        fadeProgress = 1 - bn::fixed(i)/fadespeed::MEDIUM;
        bn::blending::set_fade_alpha(bn::max(fadeProgress, bn::fixed(0)));
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
            for(bn::sprite_ptr s : explain_sprts){ s.set_blending_enabled(true);}
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_y(cursor.y() - 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
            for(bn::sprite_ptr s : explain_sprts){ s.set_blending_enabled(true);}
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
        bn::blending::set_fade_alpha(bn::min(fadeProgress, bn::fixed(1)));
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }
    
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
    constexpr bn::point mapSize(20, 22);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t tiles_arr[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, tiles_arr);
    int level = randomizer.get_int(1, 4);
    bool next_level = false, game_over = false;
    int gameover_delay = 0;

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
    bn::vector<jv::menu_option, 3> options;
    options.push_back(jv::menu_option(&val0, "Noclip"));
    options.push_back(jv::menu_option(&val1, "Invuln."));
    options.push_back(jv::menu_option(&next_level, "Next level"));

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
        next_level = false;
        gameover_delay = 0;

        // Level generation
        jv::LevelFactory(map1, level);
        level = randomizer.get_int(1, 4);

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

        if(!NoLogs){
            BN_LOG("Stack iwram: ", bn::memory::used_stack_iwram(), " Static iwram: ", bn::memory::used_static_iwram());
            BN_LOG("Alloc ewram: ", bn::memory::used_alloc_ewram(), " Static ewram: ", bn::memory::used_static_ewram());
            BN_LOG("Rom: ", bn::memory::used_rom());
            //BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());
        }

        // Fade in
        jv::fade(v_sprts, v_bgs, true);
        
        while(!next_level){
            bool objective = true;
            if(cat.is_alive()){
                cat.update(val0);
                next_level = stairs.climb(cat.rect(), cat.get_state());
            }
            
            healthbar.update();

            for(int i = 0; i < enemyCount; i++){
                v_enemies[i].update(&cat, val1);
                /*if(v_enemies[i].get_state() == State::DEAD){
                    v_sprts.erase(v_sprts.begin() + 1 + i);
                    v_enemies.erase(v_enemies.begin() + i);
                    enemyCount--;
                }*/
               objective = objective && !v_enemies[i].is_alive();
            }

            for(int i = 0; i < npcCount; i++){
                v_npcs[i].update(&cat, stairs, objective);
            }

            if(cat.is_alive()){
                if(bn::keypad::select_pressed()){ Debug::Start(options, v_sprts, v_bgs);}
            }else{
                if(gameover_delay == 120){
                    game_over = true;
                    break;
                }
                gameover_delay++;
            }
        
            jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);

            if(!NoLogs){ jv::Log_skipped_frames();}
            jv::resetcombo();
            bn::core::update();
        }

        // Fade out
        jv::fade(v_sprts, v_bgs, false, cat.is_alive() ? fadespeed::MEDIUM : fadespeed::SLOW);

        // Reset Stuff
        stairs.set_open(false);
        cat.reset();
        v_npcs.clear();
        v_points.clear();
        v_enemies.clear();
        v_sprts.erase(v_sprts.begin() + 4, v_sprts.end());

        bn::core::update();
    }
}

}

#endif