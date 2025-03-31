#include "bn_core.h"
#include "bn_log.h"
#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"


#include "jv_game.h"

int main()
{
    bn::core::init();
    bn::random randomizer;

    while(true){
        jv::game::start_scene(randomizer);
        jv::game::game_scene(randomizer);
        //jv::game::blocks_scene(randomizer);
    }
}