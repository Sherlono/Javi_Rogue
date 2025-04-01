#include "bn_core.h"
#include "bn_log.h"
#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"

#include "jv_game.h"

int main()
{
    bn::core::init();
    bn::random randomizer;
    char option = 0;

    while(true){
        //jv::game::start_scene(randomizer);
        jv::game::scene_select(randomizer, option);
        if(option < 2){ jv::game::game_scene(randomizer, option + 1);}
        else if(option == 2){ jv::game::blocks_scene(randomizer);}
    }
}