#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "bn_memory.h"
#include "bn_sprites.h"
#include "bn_bgs.h"

#include "jv_math.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_healthbar.h"
#include "jv_interface.h"
#include "jv_level_maker.h"
#include "jv_debug.h"

#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"
#include "bn_regular_bg_items_hud_item.h"

#include "bn_sprite_items_cursor.h"

namespace jv::game{
void start_scene(bn::random& randomizer, char& option){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    card.set_priority(2);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, 0);
    bg.set_priority(3);
    int y_offset = 50;
    bn::vector<bn::sprite_ptr, 64> v_text;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);
    text_generator.generate(-98, -16 + y_offset, "Select scene", v_text);
    text_generator.generate(-100, 0 + y_offset,  "Start game", v_text);
    //text_generator.generate(-100, 8 + y_offset,  "Map 2", v_text);
    text_generator.generate(-100, 8 + y_offset,  "Tile Showcase", v_text);

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-20, y_offset);
    cursor.set_bg_priority(1);
    
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
        bn::core::update();
    }
    v_text.clear();
    bn::core::update();
}

void game_scene(bn::random& randomizer){
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Music
    bn::music_items::cyberrid.play(0.25);    // Neat little song courtesy of the butano team

    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_priority(3);
    bn::regular_bg_ptr hud = bn::regular_bg_items::hud_item.create_bg(0, 0);
    hud.set_priority(0);

    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();
    bg.set_priority(2);

    // ****** Level data ******
    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t blockArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal);
    
    bn::vector<bn::point, 25> start_coords;     // Starting coordinates for all entities
    // ************************

    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    background.set_camera(cam);
    bg.set_camera(cam);
    // ************************

    // ** Universal entities **
    jv::Player cat(0, 0, bn::sprite_items::character.create_sprite(0, 0), bn::sprite_items::character.tiles_item(), cam, &randomizer, &map1);
    jv::healthbar healthbar(cat.get_maxhp_ptr(), cat.get_hp_ptr());
    jv::stairs stairs(0, 0, cam);
    
    bn::vector<jv::NPC, 1> v_npcs;
    bn::vector<jv::Enemy, 15> v_enemies;
    bn::vector<bn::sprite_ptr, 128> v_sprts;
    // ************************

    // ****** Debug data ******
    bool val0 = false; // noClip
    bn::vector<jv::menu_option, 2> options;
    options.push_back(jv::menu_option(&val0, "Noclip"));

    bn::vector<bn::regular_bg_ptr, 4> v_bgs;
    v_bgs.push_back(background);
    v_bgs.push_back(hud);
    v_bgs.push_back(bg);
    // ************************

    int level = 2;
    
    while(true){
        bool next_level = false;
        
        jv::LevelFactory(map1, level);
        jv::random_coords(start_coords, map1, randomizer);
        level = level == 1 ? 2 : 1;

        cam.set_position(start_coords[0].x(), start_coords[0].y());
        cat.set_position(start_coords[0].x(), start_coords[0].y());
        stairs.set_position(start_coords[1].x(), start_coords[1].y());

        v_sprts.push_back(cat._sprite);
        v_sprts.push_back(stairs._sprite);
        v_sprts.push_back(healthbar.bar_sprite());
        v_sprts.push_back(healthbar.corner_sprite());

        v_npcs.push_back(jv::NPC(start_coords[2].x(), start_coords[2].y(), bn::sprite_items::cow.create_sprite(0, 0), bn::sprite_items::cow.tiles_item(), cam));

        unsigned char max_enemies = 3 + randomizer.get_int(12);
        for(int i = 0; i < max_enemies; i++){
            v_enemies.push_back(jv::Enemy(start_coords[3+i].x(), start_coords[3+i].y(), bn::sprite_items::enemy.create_sprite(0, 0), bn::sprite_items::enemy.tiles_item(), cam, &randomizer, &map1));
        }
        int npcCount = v_npcs.size(), enemyCount = v_enemies.size();
        
        for(int i = 0; i < enemyCount; i++){
            v_sprts.push_back(v_enemies[i]._sprite);
        }
        for(int i = 0; i < npcCount; i++){
            v_sprts.push_back(v_npcs[i]._sprite);
        }
        
        jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
        jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);
            
        BN_LOG("Stack memory: ", bn::memory::used_stack_iwram(), " Static memory: ", bn::memory::used_static_iwram());
        BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());

        while(!next_level){
            next_level = stairs.climb(cat);

            cat.update(val0);
            for(int i = 0; i < enemyCount; i++){
                v_enemies[i].update(&cat);

                /*if(v_enemies[i].get_state() == State::DEAD){
                    v_sprts.erase(v_sprts.begin() + 1 + i);
                    v_enemies.erase(v_enemies.begin() + i);
                    enemyCount--;
                }*/
            }

            healthbar.update();

            for(int i = 0; i < npcCount; i++){
                v_npcs[i].update(&cat);
            }
            
            jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);
            if(bn::keypad::select_pressed()){ Debug::Start(options, v_sprts, v_bgs);}

            jv::Log_skipped_frames();
            jv::resetcombo();
            bn::core::update();
        }
        
        bg_map_ptr->reset();
        v_sprts.clear();
        v_npcs.clear();
        v_enemies.clear();
        start_coords.clear();
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
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();

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
    bg.set_camera(cam);
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
        
        jv::Log_skipped_frames();
        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif