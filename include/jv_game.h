#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_debug.h"
#include "jv_actors.h"
#include "jv_interface.h"
#include "jv_level_maker.h"

#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{
void start_scene(bn::random& randomizer){
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 64> v_text;
    text_generator.generate(-100, -8, "Procesors are bad at generating random", v_text);
    text_generator.generate(-100, 0,  "numbers. To compensate for this we can", v_text);
    text_generator.generate(-100, 8,  "take advantage of the humah factor.", v_text);
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

    // *** Level Generation ***
    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t blockArrfinal[cellCount*16];
    bool flipArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal, flipArrfinal);
    
    bn::vector<bn::point, 20> start_coords;
    jv::LevelFactory(map1, 2, start_coords, randomizer);
    // ************************

    // ******** Camera ********
    cam.set_position(start_coords[0].x(), start_coords[0].y());
    background.set_camera(cam);
    bg.set_camera(cam);
    // ************************

    // Characters initialization
    jv::Player cat(start_coords[0].x(), start_coords[0].y(), &randomizer, &map1, cam);
    bn::vector<jv::Enemy, 10> v_enemies;
    for(int i = 0; i < v_enemies.max_size(); i++){
        v_enemies.push_back(jv::Enemy(start_coords[2+i].x(), start_coords[2+i].y(), &randomizer, &map1, cam));
    }
    bn::vector<jv::NPC, 2> v_npcs;
    v_npcs.push_back(jv::NPC(start_coords[1].x(), start_coords[1].y(), cam));
    int npcCount = v_npcs.size(), enemyCount = v_enemies.size();
    //constexpr int actorCount = npcCount + enemyCount;
    // ************************

    // ****** Debug data ******
    bool val0 = true;  // Collision
    bool val1 = true;  // Update
    bn::vector<jv::menu_option, 2> options;
    options.push_back(jv::menu_option(&val0, "Colision"));
    options.push_back(jv::menu_option(&val1, "Update bg"));

    bn::vector<bn::regular_bg_ptr, 4> v_bgs;
    v_bgs.push_back(background);
    v_bgs.push_back(hud);
    v_bgs.push_back(bg);
    bn::vector<bn::sprite_ptr, 100> v_sprts;
    v_sprts.push_back(cat._sprite);
    for(int i = 0; i < enemyCount; i++){
        v_sprts.push_back(v_enemies[i]._sprite);
    }
    for(int i = 0; i < npcCount; i++){
        v_sprts.push_back(v_npcs[i]._sprite);
    }
    // ************************
    
    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    while(true){
        if(bn::keypad::r_pressed() && enemyCount > 0){
            v_sprts.erase(v_sprts.begin() + 1);
            v_enemies.erase(v_enemies.begin());
            enemyCount--;
        }

        cat.update(cam, &val0);
        for(int i = 0; i < enemyCount; i++){
            v_enemies[i].update(&cat);
        }
        for(int i = 0; i < npcCount; i++){
            v_npcs[i].update(&cat);
        }
        
        if(val1){ jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);}
        if(bn::keypad::start_pressed()){ jv::Debug::Start(options, v_bgs, v_sprts);}

        jv::Log_skipped_frames();
        jv::resetcombo();
        bn::core::update();
    }
}

void blocks_scene(bn::random& randomizer){
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    bn::vector<bn::sprite_ptr, 64> numbers;
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

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t blockArrfinal[cellCount*16];
    bool flipArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal, flipArrfinal);
    
    bn::vector<bn::point, 1> start_coords;
    jv::LevelFactory(map1, 0, start_coords, randomizer);

    // Characters initialization
    jv::Player cat(0, 0, &randomizer, &map1, cam);
    cam.set_position(120, 80);
    cat.set_visible(false);
    // ************************

    // ****** Debug data ******
    bool val0 = false;
    bn::vector<bn::sprite_ptr, 128> sprts;
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            if(bamod(x, 2) == 0){
                int num = (x + y*10)/2;
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
        cat.update(cam, &val0);
        
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