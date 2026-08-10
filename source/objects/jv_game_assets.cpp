#include "jv_game_assets.h"

#include "bn_log.h"

#include "jv_global.h"

namespace jv{
Graphics_Manager::Graphics_Manager(){
    BN_LOG("Ding!");
    for(int i = 0; i < MAX_ACTORS; i++){
        us_graphic_indexes.insert(i);
        BN_LOG("Front key: ", *us_graphic_indexes.begin());
    }
}

void Graphics_Manager::create_sprite(){
    BN_LOG("Used key: ", Global::Graphics_Manager().front_key());
    um_graphics.insert(generate_key(), Actor::Graphics());
}

void Graphics_Manager::erase_sprite(int key){
    us_graphic_indexes.insert(key);
    um_graphics.erase(key);
}

void Graphics_Manager::graphics_update(){
    for(auto graphics : um_graphics) if(!graphics.second.animation.done()) graphics.second.animation.update();
}



GameAssets::GameAssets(bn::random& r): randomizer(r), cat(bn::point(0, 0)){
    cat.set_enemies_ptr(&v_enemies);
}

void GameAssets::enemies_update(){
    for(int i = 0; i < v_enemies.size(); i++){
        v_enemies[i].update();
        
        if(v_enemies[i].get_state() == Actor::State::DEAD) [[unlikely]] {
            int item_check = Global::Random().get_int(0, 3);
            if(item_check == 1){
                v_scene_items.push_back(new jv::Potion(v_enemies[i].x(), v_enemies[i].y()));
            }else if(item_check == 2){
                v_scene_items.push_back(new jv::Key(v_enemies[i].x(), v_enemies[i].y()));
            }
            //delete v_enemies[i];
            v_enemies.erase(v_enemies.begin() + i);
        }
    }
}

void GameAssets::npcs_update(){
    for(int i = 0; i < v_npcs.size(); i++){
        v_npcs[i].update();
    }
}

void GameAssets::set_all_visible(bool visible){
    cat.set_visible(visible);
    healthbar.set_visible(visible);
    npcs_set_visible(visible);
    enemies_set_visible(visible);
    items_set_visible(visible);
    projectiles_set_visible(visible);
}

void GameAssets::update(){
    enemies_update();
    projectiles_update();
    npcs_update();
    healthbar.update();
}

}