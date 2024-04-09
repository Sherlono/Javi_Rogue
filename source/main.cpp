#include "bn_music_items.h"
#include "bn_regular_bg_items_bg.h"

#include "jv_game.h"

int main()
{
    bn::core::init();
    //bn::random randomizer;
    //bn::random* random_ptr = &randomizer;
    //bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);

    bn::music_items::cyberrid.play(0.5);    // Neat little song courtesy of the butano team

    while(true){
        //jv::game::game_scene(cam, random_ptr);
        bn::core::update();
    }
}