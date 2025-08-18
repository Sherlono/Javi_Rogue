#include "bn_core.h"
#include "bn_assert.h"
#include "bn_regular_bg_ptr.h"

#include "jv_game.h"
#include "jv_credits.h"

#if DEV_ENABLED
    #include "jv_dev_tools.h"
#endif

int main()
{
    bn::core::init();
    bn::random randomizer;
    
    #if !DEV_ENABLED
        jv::game::intro_scene();
    #endif
    while(true){
        switch(jv::game::start_scene(randomizer)){
            case 0:{
                jv::game::MainGame game(randomizer);
                break;
                }
            #if !DEV_ENABLED
                case 1:
                    jv::credits::credits_scene();
                    break;
            #else
                case 1:
                    jv::dev::blocks_scene();
                    break;
                case 2:
                    jv::dev::tile_scene();
                    break;
            #endif

            default:
                BN_ASSERT(false, "Invalid Scene");
                break;
        }
    }
}