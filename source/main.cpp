#include "bn_core.h"
#include "bn_assert.h"
#include "bn_regular_bg_ptr.h"

#include "jv_game.h"
#include "jv_credits.h"
#include "jv_intro_movie.h"

#if DEV_ENABLED
    #include "jv_dev_tools.h"
#endif

int main()
{
    bn::core::init();
    bn::random randomizer;
    
    #if !DEV_ENABLED
        jv::scenes::intro_scene();
        {jv::scenes::IntroMovie Start;}
    #endif
    while(true){
        switch(jv::scenes::start_scene(randomizer)){
            case 0:{
                jv::scenes::MainGame Start(randomizer);
                break;
            }
            #if !DEV_ENABLED
            case 1:
                jv::scenes::credits_scene();
                break;
            case 2:{
                jv::scenes::IntroMovie Start;
                break;
            }
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