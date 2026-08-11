#include "jv_game_assets.h"

#include "bn_log.h"

#include "jv_global.h"

namespace jv{
Graphics_Manager::Graphics_Manager(){
    BN_LOG("Graphics Manager Constructor Called.");
    for(int i = 0; i < MAX_ACTORS; i++){
        us_graphic_indexes.insert(i);
    }
}

void Graphics_Manager::create_sprite(key_t& actor_key){
    key_t key = *us_graphic_indexes.begin();
    us_graphic_indexes.erase(key);                  // Take keys
    um_graphics.insert(key, Actor::Graphics());     // Create graphics
    actor_key = key;
    //BN_LOG("Sprite created.", " | Key: ", key, " | Used key: ", actor_key);
}

void Graphics_Manager::erase_sprite(key_t& actor_key){
    us_graphic_indexes.insert(actor_key); // Return key
    um_graphics.erase(actor_key);         // Clear graphics
    //BN_LOG("Sprite erased.", " Erased key: ", actor_key);
    actor_key = 255;
}

void Graphics_Manager::graphics_update(){
    for(auto it = um_graphics.begin(); it != um_graphics.end(); it++) if(!it->second.animation.done()) it->second.animation.update();
}



GameAssets::GameAssets(bn::random& r): randomizer(r), cat(bn::point(0, 0)) {}

void GameAssets::clear_objects(bool clear_npcs){
    if(clear_npcs){
        //for(auto npc : v_npcs) delete npc;
        for(NPC* npc : v_npcs) Global::Graphics_Manager().erase_sprite(npc->graphics_key);
        v_npcs.clear();
    }
    //for(auto enemy : v_enemies) delete enemy;
    for(Enemy* enemy : v_enemies) Global::Graphics_Manager().erase_sprite(enemy->graphics_key);
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
    healthbar.set_visible(visible);
    items_set_visible(visible);
    projectiles_set_visible(visible);
    for(auto it = Global::Graphics_Manager().um_graphics.begin(); it != Global::Graphics_Manager().um_graphics.end(); it++) it->second.set_visible(visible);
}

void GameAssets::update(){
    enemies_update();
    projectiles_update();
    npcs_update();
    healthbar.update();
    graphicsManager.graphics_update();
}

}