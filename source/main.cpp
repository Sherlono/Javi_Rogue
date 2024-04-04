#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprites.h"

#include "common_variable_8x8_sprite_font.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "bn_music_items.h"

#include "bn_regular_bg_items_bg.h"
#include "jv_constants.h"
#include "jv_environment.h"
#include "jv_interface.h"
#include "jv_actors.h"
#include "jv_math.h"

int main()
{
    bn::core::init();
    bn::point initial_pos (16 + 2*32,16 + 2*32);
    //bn::point initial_pos (-1*32, 0);
    jv::Player player1(initial_pos.x(), initial_pos.y());
    bn::camera_ptr cam = bn::camera_ptr::create(initial_pos.x(), initial_pos.y());

    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);

    bn::music_items::cyberrid.play(0.5);    // Neat little song courtesy of the butano team

    bn::vector<jv::para, MAX_BLOCKS> para_v;
    bn::vector<jv::Block*, MAX_BLOCKS> block_holder;

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

    /*bn::point map_shape(8, 11);
    cuchar_t block_array[9 * 10] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,
                                    10, 11, 12, 13, 14, 15, 16, 17, 18,
                                    19, 20, 21, 22, 23, 24, 25, 26, 27,
                                    28, 29, 30, 31, 32, 33, 34, 35, 36,
                                    37, 38, 39, 40, 41, 42, 43, 44, 45,
                                    46, 47, 48, 49, 50, 51, 52, 53, 54,
                                    55, 56, 57, 58, 59, 60, 61, 62, 63,
                                    64, 65, 66, 67, 68, 69, 70, 71, 72,
                                    73, 74, 75, 76, 77, 78, 79, 80, 81};*/
    
    jv::GameMap map1(map_shape.x(), map_shape.y(), block_array);

    /*// On screen text stuff (mostly for debugging)
    bn::vector<bn::sprite_ptr, 14> v_text;
    bn::string<32> mytext = "";
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);

    jv::Block* myblock;
    myblock = new jv::Wall(0, 0, cam, 0);*/
    
    jv::LevelMaker::update(map1, cam, para_v, block_holder);


    while(true){
        /*mytext = "Sprite: " + bn::to_string<16>(jv::block_scroll(myblock, cam));
        text_generator.generate(-110, -65, mytext, v_text);*/

        player1.move_player(cam, para_v);
        
        jv::LevelMaker::update(map1, cam, para_v, block_holder);
        bn::core::update();
        //v_text.clear();
    }
}