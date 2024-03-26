#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_camera_actions.h"
#include "bn_regular_bg_ptr.h"

/*#include "common_variable_8x8_sprite_font.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"*/

#include "bn_regular_bg_items_bg.h"
#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_interface.h"
#include "jv_actors.h"
#include "jv_math.h"


int main()
{
    bn::core::init();
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    bn::point initial_pos (16 + 2*32, 2*32);
    bn::camera_ptr cam = bn::camera_ptr::create(initial_pos.x(), initial_pos.y());
    jv::Player player1(initial_pos.x(), initial_pos.y());

    bn::vector<jv::para, constants::max_blocks> para_v;
    bn::vector<jv::Block*, constants::max_blocks> block_holder;
    unsigned char block_array[constants::max_blocks] = {0, 17, 21, 21, 18, 1, 99, 99,
                                                        4, 22, 30, 31, 23, 5, 99, 99,
                                                        6, 44, 46, 47, 45, 7, 99, 99,
                                                        8, 38, 48, 46, 39, 9, 99, 99,
                                                        10, 59, 55, 56, 60, 11, 99, 99,
                                                        99, 14, 16, 16, 15, 99, 99, 99};
    
    /*unsigned char block_array[constants::max_blocks] = {0, 1, 2, 3, 4, 5,
                                                          6, 7, 8, 9, 10, 11,
                                                          12, 13, 14, 15, 16, 17,
                                                          18, 19, 20, 21, 22, 23,
                                                          24, 25, 26, 27, 28, 29,
                                                          30, 31, 32, 33, 34, 35,
                                                          36, 37, 38, 39, 40, 41,
                                                          42, 43, 44, 45, 46, 47,
                                                          48, 49, 50, 51, 52, 53,
                                                          54, 55, 56, 57, 58, 59,
                                                          60, 61, 62, 63, 64, 65};*/

    /*unsigned char block_array[constants::max_blocks] = {24, 25, 26, 27, 28, 29,
                                                          30, 31, 32, 33, 34, 35,
                                                          36, 37, 38, 39, 40, 41,
                                                          42, 43, 44, 45, 46, 47,
                                                          48, 49, 50, 51, 52, 53,
                                                          54, 55, 56, 57, 58, 59,
                                                          54, 55, 56, 57, 58, 59,
                                                          54, 55, 56, 57, 58, 59};*/
                                     
    //jv::LevelGenerator(8, 6, cam, para_v, block_holder, block_array);

    /*bn::vector<bn::sprite_ptr, 8> v_text;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);*/

    while(true){
        //text_generator.generate(-80, -55, bn::to_string<8>(i), v_text);
        jv::LevelGenerator(8, 6, cam, para_v, block_holder, block_array);
        player1.move_player(cam, para_v);
        
        bn::core::update();
        //v_text.clear();
    }
}