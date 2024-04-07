#include "bn_music_items.h"
#include "bn_regular_bg_items_bg.h"

#include "jv_game.h"

int main()
{
    bn::core::init();

    const bn::point initial_pos (16 + 2*32,16 + 2*32);
    bn::camera_ptr cam = bn::camera_ptr::create(initial_pos.x(), initial_pos.y());
    jv::Player cat(initial_pos.x(), initial_pos.y());

    bn::music_items::cyberrid.play(0.5);    // Neat little song courtesy of the butano team

    while(true){
        jv::game::game_scene(cat, cam);
    }
}