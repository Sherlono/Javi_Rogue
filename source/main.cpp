#include "bn_core.h"
#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"

#include "jv_game.h"
#include "jv_dev_tools.h"

int main()
{
    bn::core::init();
    bn::random randomizer;
    char option = 0;
    
    jv::game::intro_scene();

    while(true){
        jv::game::start_scene(randomizer, option);
        switch(option){
            case 0:
                jv::game::game_scene(randomizer);
                break;
            case 1:
                jv::dev::blocks_scene();
                break;
            case 2:
                jv::dev::tile_scene();
                break;
            default:
                break;
        }
    }
}