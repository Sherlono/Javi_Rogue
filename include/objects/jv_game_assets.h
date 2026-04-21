#ifndef JV_GAME_ASSETS_H
#define JV_GAME_ASSETS_H

#include "bn_vector.h"
#include "bn_random.h"
#include "bn_camera_ptr.h"

#include "jv_fog.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_projectile.h"
#include "jv_constants.h"

namespace jv{
struct GameAssets{
    using NPCs_vector_t = bn::vector<NPC, 1>;
    using enemies_vector_t = bn::vector<Enemy*, MAX_ENEMIES>;
    using items_vector_t = bn::vector<Item*, MAX_ENEMIES>;
    using projectiles_vector_t = bn::vector<Projectile*, MAX_ENEMIES>;

    GameAssets(bn::camera_ptr& c, bn::random& r);
    
    void clear_objects(){
        for(auto enemy : v_enemies) delete enemy;
        v_enemies.clear();
        for(auto item : v_scene_items) delete item;
        v_scene_items.clear();
        for(auto projectile : v_projectiles) delete projectile;
        v_projectiles.clear();
    }

    void scene_items_update(){
        for(int i = 0; i < v_scene_items.size(); i++){
            if(!v_scene_items[i]->gotten()){
                v_scene_items[i]->update();
            }else{
                delete v_scene_items[i];
                v_scene_items.erase(v_scene_items.begin() + i);
            }
        }
    }

    void enemies_update();
    void projectiles_update(){
        for(int i = 0; i < v_projectiles.size(); i++){
            if(v_projectiles[i]->update()){
                delete v_projectiles[i];
                v_projectiles.erase(v_projectiles.begin() + i);
            }
        }
    }

    inline void npcs_set_visible(bool visible){
        for(auto npc : v_npcs) npc.set_visible(visible);
    }
    inline void enemies_set_visible(bool visible){
        for(auto enemy : v_enemies) enemy->set_visible(visible);
    }
    inline void items_set_visible(bool visible){
        for(auto item : v_scene_items) item->set_visible(visible);
    }
    inline void projectiles_set_visible(bool visible){
        for(auto projectile : v_projectiles){ projectile->set_visible(visible);}
    }

    void set_all_visible(bool visible){
        cat.set_visible(visible);
        npcs_set_visible(visible);
        enemies_set_visible(visible);
        items_set_visible(visible);
        projectiles_set_visible(visible);
    }

    //bn::camera_ptr cam;
    bn::random& randomizer;
    
    Player cat;
    Stairs stairs;
    Fog<MAX_ROOMS> fog;

    NPCs_vector_t v_npcs;
    enemies_vector_t v_enemies;
    items_vector_t v_scene_items;
    projectiles_vector_t v_projectiles;
};

}


#endif
