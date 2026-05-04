#include "bn_core.h"
#include "bn_assert.h"

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
    jv::Scenes::IntroScene();
    jv::Scenes::IntroMovie::Start();
    #endif

    while(true){
        switch(jv::Scenes::StartScene(randomizer)){
            case jv::Scenes::Tag::Main:{
                jv::Scenes::MainGame::Start(randomizer);
                break;
            }
            #if DEV_ENABLED
            case jv::Scenes::Tag::Blocks:{
                jv::Scenes::BlocksScene::Start();
                break;
            }
            /*jv::Scenes::Tag::Tiles:{
                jv::dev::tile_scene();
                break;
            }*/
            #else
            case jv::Scenes::Tag::Credits:{
                jv::Scenes::CreditsScene::Start();
                break;
            }
            case jv::Scenes::Tag::Restart:{
                jv::Scenes::IntroMovie::Start();
                break;
            }
            #endif

            default:{
                BN_ERROR("Invalid Scene.");
                break;
            }
        }
        bn::core::update();
    }
}