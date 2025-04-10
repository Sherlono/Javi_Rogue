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

    jv::game::start_scene(randomizer, option);
    if(option == 1){ jv::game::blocks_scene();}

    while(true){
        jv::game::game_scene(randomizer);
    }
}