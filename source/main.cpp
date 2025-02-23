#include "bn_core.h"
#include "bn_log.h"
#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_bg.h"

#include "jv_game.h"

int main()
{
    bn::core::init();
    bn::random randomizer;
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    while(true){
        jv::game::start_scene(text_generator, randomizer);
        jv::game::game_scene(cam, text_generator, randomizer);
    }
}