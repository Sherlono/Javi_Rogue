#ifndef JV_GAME_H
#define JV_GAME_H

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"
#include "jv_interface.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{

void game_scene(bn::camera_ptr& cam, bn::random* random_ptr){
    // Player preparation
    const bn::point start_pos (16 + 1*32,16 + 1*32);
    cam.set_position(start_pos.x(), start_pos.y());
    jv::Player cat(start_pos.x(), start_pos.y(), random_ptr);
    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);
    bn::regular_bg_ptr hud = bn::regular_bg_items::hud_item.create_bg(0, 0);
    hud.set_priority(0);
    
    // NPCs
    jv::NPC cow(2*32, 7*32 , cam);
    jv::Enemy badcat(16*32, 2*32, cam, random_ptr);

    bn::vector<jv::para, MAX_PARA> para_v;
    bn::vector<bn::sprite_ptr, MAX_BLOCKS> block_v;

    /*bn::point map_shape(23, 10);
    uchar_t block_array[230] = {1 , 24, 28, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 25, 2 ,
                                5 , 29, 37, 38, 30, 28, 28, 28, 28, 29, 37, 49, 49, 49, 49, 49, 49, 49, 49, 49, 38, 30, 6 ,
                                7 , 51, 53, 54, 78, 72, 72, 72, 72, 77, 54, 53, 53, 53, 53, 53, 55, 53, 54, 53, 53, 52, 8 ,
                                9 , 45, 55, 53, 46, 18, 23, 23, 19, 45, 55, 53, 53, 55, 53, 53, 53, 55, 53, 53, 55, 46, 10,
                                11, 15, 21, 22, 16, 12, 0 , 0 , 11, 15, 21, 69, 20, 20, 20, 20, 20, 20, 20, 20, 22, 16, 12, 
                                1 , 24, 28, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 56, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 28, 25, 2 ,
                                5 , 29, 37, 38, 30, 28, 28, 28, 28, 29, 37, 63, 38, 30, 28, 28, 28, 28, 29, 37, 38, 30, 6 ,
                                7 , 51, 53, 54, 78, 72, 72, 72, 72, 77, 54, 53, 53, 78, 72, 72, 72, 72, 77, 53, 53, 52, 8 ,
                                9 , 45, 55, 53, 46, 18, 23, 23, 19, 45, 55, 53, 53, 46, 18, 23, 23, 19, 45, 53, 55, 46, 10,
                                11, 15, 21, 22, 16, 12, 0 , 0 , 11, 15, 21, 20, 22, 16, 12, 0 , 0 , 11, 15, 21, 22, 16, 12};*/

    bn::point map_shape(15, 10);
    uchar_t block_array[150] = {0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

    // On screen text stuff (mostly for debugging)
    /*bn::vector<bn::sprite_ptr, 14> text_v;
    bn::string<32> mytext = "";
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);*/

    /*jv::Block* myblock;
    myblock = new jv::Wall(0, 0, cam, 1);
    para_v.push_back(myblock->get_para());*/
    
    // Level management
    jv::GameMap map1(map_shape.x(), map_shape.y(), block_array);
    //map1.insert_room(jv::RoomPrefab(1), bn::point(0, 0));
    map1.insert_room(jv::RoomPrefab(3), bn::point(0, 0));

    jv::LevelMaker::init(map1, cam, para_v, block_v);

    while(true){
        /*mytext = "lmf: " + bn::to_string<16>(bn::core::last_missed_frames());
        text_generator.generate(-110, -65, mytext, text_v);
        mytext = "catY: " + bn::to_string<16>(cat.y());
        text_generator.generate(-110, -55, mytext, text_v);*/

        cat.update(cam, para_v);
        badcat.update(cat, para_v);
        cow.update(cat);

        jv::LevelMaker::update(map1, cam, block_v);
        jv::resetcombo();
        bn::core::update();
        //text_v.clear();
    }
}
}
#endif