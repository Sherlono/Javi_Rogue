#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_regular_bg_ptr.h"

//#include "common_variable_8x8_sprite_font.h"
//#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "bn_regular_bg_items_bg.h"
#include "jv_environment.h"
#include "jv_interface.h"
#include "jv_actors.h"
#include "jv_math.h"

int main()
{
    bn::core::init();
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    bn::point initial_pos (16 + 2*32, 16 + 2*32);
    bn::camera_ptr cam = bn::camera_ptr::create(initial_pos.x(), initial_pos.y());
    jv::Player player1(initial_pos.x(), initial_pos.y());
    bn::vector<jv::para, 50> para_v;
    bn::vector<jv::Block*, 50> block_holder;
    unsigned char block_array[50] = {0, 20, 19, 19, 21, 1,
                                     4, 25, 33, 32, 26, 5,
                                     10, 29, 24, 24, 30, 11,
                                     12, 35, 24, 24, 36, 13,
                                     6, 40, 45, 46, 41, 7,
                                     99, 15, 14, 14, 16, 99,
                                     99, 15, 14, 14, 16, 99};
    jv::LevelGenerator(6, 6, cam, para_v, block_holder, block_array);

    /*bn::vector<bn::sprite_ptr, 8> v_text;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_bg_priority(0);

    /*jv::Wall2 wall1(0, 1, cam, para_v, 0);
    jv::Wall8 wall2(1, 1, cam, para_v, 0);
    jv::Wall6 wall3(2, 1, cam, para_v);
    jv::Wall6 wall4(3, 1, cam, para_v);
    jv::Wall8 wall5(4, 1, cam, para_v, 1);
    jv::Wall2 wall6(5, 1, cam, para_v, 1);

    jv::Wall2 wall7(0, 2, cam, para_v, 4);
    jv::Floor floor1(1, 2, cam, 1);
    jv::Floor floor2(2, 2, cam, 9);
    jv::Floor floor16(3, 2, cam, 8);
    jv::Floor floor3(4, 2, cam, 2);
    jv::Wall2 wall8(5, 2, cam, para_v, 5);

    jv::Wall2 wall9(0, 3, cam, para_v, 10);
    jv::Floor floor4(1, 3, cam, 5);
    jv::Floor floor5(2, 3, cam, 0);
    jv::Floor floor6(3, 3, cam, 0);
    jv::Floor floor7(4, 3, cam, 6);
    jv::Wall2 wall10(5, 3, cam, para_v, 11);

    jv::Wall2 wall11(0, 4, cam, para_v, 12);
    jv::Floor floor8(1, 4, cam, 11);
    jv::Floor floor9(2, 4, cam, 0);
    jv::Floor floor10(3, 4, cam, 0);
    jv::Floor floor11(4, 4, cam, 12);
    jv::Wall2 wall12(5, 4, cam, para_v, 13);
    
    jv::Wall2 wall13(0, 5, cam, para_v, 6);
    jv::Floor floor12(1, 5, cam, 16);
    jv::Floor floor13(2, 5, cam, 21);
    jv::Floor floor14(3, 5, cam, 22);
    jv::Floor floor15(4, 5, cam, 17);
    jv::Wall2 wall14(5, 5, cam, para_v, 7);

    jv::Wall4 wall15(1, 6, cam, para_v, 0);
    jv::Wall2 wall16(2, 6, cam, para_v, 14);
    jv::Wall2 wall17(3, 6, cam, para_v, 14);
    jv::Wall4 wall18(4, 6, cam, para_v, 1);*/

    while(true){
        //text_generator.generate(-80, -55, bn::to_string<8>(i), v_text);
        player1.move_player(cam, para_v);

        bn::core::update();
        //v_text.clear();
    }
}