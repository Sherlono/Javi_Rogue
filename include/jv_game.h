#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_debug.h"
#include "jv_actors.h"
#include "jv_interface.h"
#include "jv_environment.h"
#include "jv_level_maker.h"

#include "bn_sprite_items_ball.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{
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

    constexpr bn::point mapSize(20, 20);
    constexpr int cellCount = mapSize.x()*mapSize.y();

    uchar_t blockArrfinal[cellCount*16];
    bool flipArrfinal[cellCount*16];
    game_map map1(mapSize.x()*4, mapSize.y()*4, blockArrfinal, flipArrfinal);
    
    bn::vector<bn::point, 3> start_points;
    
    {
    uchar_t blockArr[cellCount] = {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
         0, 0, 9, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 9, 0, 0, 0,20,
         0, 0, 1,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0, 0, 0,20,
         0, 0, 1,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0, 0, 0,20,
         0, 0, 1,20,20,20,20,13,10,20,10,13,20,20,20, 1, 0, 0, 0,20,
         0, 0,17,11,10,20,11,17, 0,20, 0,17,11,20,11,17, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20,20,20,20,20, 0, 0, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 0, 0,20,20,20,20,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 9, 3,20, 3, 9, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 0, 0, 0,20, 0, 1,20,20,20, 1, 0,20, 0, 0, 0, 0, 0,20,
         0, 0, 9, 3, 2,20, 2, 5,20,20,20, 5, 2,20, 2, 3, 9, 0, 0,20,
         0, 0, 1,20,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0, 0,20,
         0, 0, 1,20,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0, 0,20,
         0, 0, 1,20,20,20,20,20,20,20,20,20,20,20,20,20, 1, 0, 0,20,
         0, 0,17,11,10,10,10,10,10,10,10,10,10,10,10,11,17, 0, 0,20,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    random_coords(start_points, randomizer, blockArr, mapSize.x(), mapSize.y());
         
    bool flipArr[cellCount] = {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
         1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
         
    for(int y = 0; y < mapSize.y(); y++){
        for(int x = 0; x < mapSize.x(); x++){
            int index = x + y*mapSize.x();
            FloorFactory(map1, bn::point(x*4, y*4), blockArr[index], flipArr[index]);
        }
    }
    }

    // Characters initialization
    jv::Player cat(0, 0, randomizer, map1, cam);
    cam.set_position(start_points[0]);
    cat.set_position(start_points[0]);
    jv::NPC cow(0, 0, cam);
    cow.set_position(start_points[1]);
    jv::Enemy badcat(0, 0, randomizer, map1, cam);
    badcat.set_position(start_points[2]);
    // ************************

    // ****** Debug data ******
    bool val0 = true;  // Collision
    bool val1 = true;  // Update
    //int val2 = 0;
    bn::vector<jv::menu_option, 10> options;
    options.push_back(jv::menu_option(&val0, "Colision"));
    options.push_back(jv::menu_option(&val1, "Update bg"));
    //options.push_back(jv::menu_option(&val2, "Na"));

    bn::vector<bn::regular_bg_ptr, 4> bgs;
    bgs.push_back(background);
    bgs.push_back(bg);
    bn::vector<bn::sprite_ptr, 20> sprts;
    sprts.push_back(cat._sprite);
    sprts.push_back(badcat._sprite);
    sprts.push_back(cow._sprite);
    /*for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            bn::sprite_ptr ball = bn::sprite_items::ball.create_sprite(x , y);
            ball.set_camera(cam);
            sprts.push_back(ball);
        }
    }*/
    // ************************
    
    
    jv::LevelMaker::init(cam, map1, bg_map_ptr, bg_map);
    
    while(true){
        cat.update(cam, val0);
        cow.update(cat);
        badcat.update(cat);
        
        if(val1){ jv::LevelMaker::update(cam, map1, bg_map_ptr, bg_map);}
        if(bn::keypad::start_pressed()){ jv::Debug::Start(options, bgs, sprts);}

        jv::Log_skipped_frames();
        jv::resetcombo();
        bn::core::update();
    }
}
}

#endif