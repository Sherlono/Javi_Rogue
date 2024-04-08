#ifndef JV_GAME_H
#define JV_GAME_H

#include "jv_interface.h"

namespace jv::game{

void game_scene(jv::Player& cat, bn::camera_ptr& cam){
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);

    bn::vector<jv::para, MAX_PARA> para_v;
    bn::vector<jv::Block*, MAX_BLOCKS> block_v;

    bn::point map_shape(23, 10);
    cuchar_t block_array[23 * 10] = {1 , 24, 28, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 25, 2 ,
                                     5 , 29, 37, 38, 30, 28, 28, 28, 28, 29, 37, 49, 49, 49, 49, 49, 49, 49, 49, 49, 38, 30, 6 ,
                                     7 , 51, 53, 54, 78, 72, 72, 72, 72, 77, 54, 53, 53, 53, 53, 53, 55, 53, 54, 53, 53, 52, 8 ,
                                     9 , 45, 55, 53, 46, 18, 23, 23, 19, 45, 55, 53, 53, 55, 53, 53, 53, 55, 53, 53, 55, 46, 10,
                                     11, 15, 21, 22, 16, 12, 0 , 0 , 11, 15, 21, 69, 20, 20, 20, 20, 20, 20, 20, 20, 22, 16, 12, 
                                     1 , 24, 28, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 56, 28, 25, 2 , 0 , 0 , 1 , 24, 28, 28, 25, 2 ,
                                     5 , 29, 37, 38, 30, 28, 28, 28, 28, 29, 37, 63, 38, 30, 28, 28, 28, 28, 29, 37, 38, 30, 6 ,
                                     7 , 51, 53, 54, 78, 72, 72, 72, 72, 77, 54, 53, 53, 78, 72, 72, 72, 72, 77, 53, 53, 52, 8 ,
                                     9 , 45, 55, 53, 46, 18, 23, 23, 19, 45, 55, 53, 53, 46, 18, 23, 23, 19, 45, 53, 55, 46, 10,
                                     11, 15, 21, 22, 16, 12, 0 , 0 , 11, 15, 21, 20, 22, 16, 12, 0 , 0 , 11, 15, 21, 22, 16, 12};

    /*bn::point map_shape(9, 10);
    cuchar_t block_array[9 * 10] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,
                                    10, 11, 12, 13, 14, 15, 16, 17, 18,
                                    19, 20, 21, 22, 23, 24, 25, 26, 27,
                                    28, 29, 30, 31, 32, 33, 34, 35, 36,
                                    37, 38, 39, 40, 41, 42, 43, 44, 45,
                                    46, 47, 48, 49, 50, 51, 52, 53, 54,
                                    55, 56, 57, 58, 59, 60, 61, 62, 63,
                                    64, 65, 66, 67, 68, 69, 70, 71, 72,
                                    73, 74, 75, 76, 77, 78, 79, 80, 81};*/
    
    // On screen text stuff (mostly for debugging)
    /*bn::vector<bn::sprite_ptr, 14> text_v;
    bn::string<32> mytext = "";
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);*/

    /*jv::Block* myblock;
    myblock = new jv::Wall(0, 0, cam, 1);
    para_v.push_back(myblock->get_para());*/
    
    jv::GameMap map1(map_shape.x(), map_shape.y(), block_array);
    jv::LevelMaker::init(map1, cam, para_v, block_v);
    jv::NPC cow(2*32, 7*32 , cam);
    jv::Enemy badcat(16*32, 2*32, cam);

    while(true){
        /*mytext = "cowY: " + bn::to_string<16>(cow.y());
        text_generator.generate(-110, -65, mytext, text_v);
        mytext = "catY: " + bn::to_string<16>(cat.y());
        text_generator.generate(-110, -55, mytext, text_v);*/

        cat.update(cam, para_v);
        badcat.update(cat, para_v);
        cow.update(cat);
        jv::LevelMaker::update(map1, cam, block_v);
        bn::core::update();
        jv::resetcombo();
        //text_v.clear();
    }
}


}
#endif