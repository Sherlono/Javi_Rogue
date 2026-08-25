#include "jv_game_assets.h"

#include "bn_log.h"

#include "jv_global.h"

namespace jv{
GameAssets::GameAssets(bn::random& r): randomizer(r), cat(bn::point(0, 0)) {}

void GameAssets::clear_objects(bool clear_npcs){
    Graphics_Manager& graphics_m = Global::Graphics_Manager();
    if(clear_npcs){
        for(NPC* npc : v_npcs){
            if(graphics_m.find(npc->graphics_key)) graphics_m.erase_sprite(npc->graphics_key);
            delete npc;
        }
        v_npcs.clear();
    }
    for(Enemy* enemy : v_enemies){
        if(graphics_m.find(enemy->graphics_key)) graphics_m.erase_sprite(enemy->graphics_key);
        delete enemy;
    }
    v_enemies.clear();

    for(auto item : v_scene_items) delete item;
    v_scene_items.clear();
    for(auto projectile : v_projectiles) delete projectile;
    v_projectiles.clear();
    fog.clear();
}

void GameAssets::enemies_update(){
    for(int i = 0; i < v_enemies.size(); i++){
        v_enemies[i]->update();
        
        if(v_enemies[i]->get_state() == Actor::State::DEAD) [[unlikely]] {
            int item_check = Global::Random().get_int(0, 3);
            if(item_check == 1){
                v_scene_items.push_back(new jv::Potion(v_enemies[i]->x(), v_enemies[i]->y()));
            }else if(item_check == 2){
                v_scene_items.push_back(new jv::Key(v_enemies[i]->x(), v_enemies[i]->y()));
            }
            
            Global::Graphics_Manager().erase_sprite(v_enemies[i]->graphics_key);
            delete v_enemies[i];
            v_enemies.erase(v_enemies.begin() + i);
        }
    }
}

void GameAssets::npcs_update(){
    for(int i = 0; i < v_npcs.size(); i++){
        v_npcs[i]->update();
    }
}

void GameAssets::set_all_visible(bool visible){
    items_set_visible(visible);
    projectiles_set_visible(visible);
    healthbar.set_visible(visible);
    Global::Graphics_Manager().set_all_visible(visible);
}

void GameAssets::update(){
    npcs_update();
    enemies_update();
    projectiles_update();
    healthbar.update();
    graphicsManager.update();
    //if(tree.has_value()) tree.value().update();
}

}