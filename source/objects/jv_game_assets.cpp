#include "jv_game_assets.h"

namespace jv{
    
GameAssets::GameAssets(bn::camera_ptr& c, bn::random& r): randomizer(r), cat(bn::point(0, 0), c)
{
    cat.set_enemies_ptr(&v_enemies);
}

void GameAssets::enemies_update(){
    for(int i = 0; i < v_enemies.size(); i++){
        v_enemies[i]->update();
        //objective = objective && !_v_enemies[i]->alive();
        if(v_enemies[i]->get_state() == Actor::State::DEAD) [[unlikely]] {
            int item_check = Global::Random().get_int(0, 3);
            if(item_check == 1){
                v_scene_items.push_back(new jv::Potion(v_enemies[i]->x(), v_enemies[i]->y()));
            }else if(item_check == 2){
                v_scene_items.push_back(new jv::Key(v_enemies[i]->x(), v_enemies[i]->y()));
            }
            delete v_enemies[i];
            v_enemies.erase(v_enemies.begin() + i);
        }
    }
}


}