#ifndef JV_GAME_ASSETS_H
#define JV_GAME_ASSETS_H

#include "bn_random.h"

#include "bn_log.h"
#include "bn_vector.h"

#include "jv_fog.h"
#include "jv_items.h"
#include "jv_actors.h"
#include "jv_player.h"
#include "jv_stairs.h"
#include "jv_constants.h"
#include "jv_healthbar.h"
#include "jv_projectile.h"
#include "jv_graphics_manager.h"

namespace jv{
struct GameAssets{
    using NPCs_vector_t = bn::vector<NPC*, NPCS_COUNT>;
    using enemies_vector_t = bn::vector<Enemy*, MAX_ENEMIES>;
    using items_vector_t = bn::vector<Item*, MAX_ENEMIES>;
    using projectiles_vector_t = bn::vector<Projectile*, MAX_ENEMIES>;

    GameAssets() = delete;
    GameAssets(GameAssets &other) = delete;
    GameAssets(GameAssets &&other) = delete;
    GameAssets(bn::random& r);
    
    void clear_objects(bool clear_npcs = true);
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
    void npcs_update();
    void projectiles_update(){
        for(int i = 0; i < v_projectiles.size(); i++){
            if(v_projectiles[i]->update()){
                delete v_projectiles[i];
                v_projectiles.erase(v_projectiles.begin() + i);
            }
        }
    }

    inline void items_set_visible(bool visible){
        for(auto item : v_scene_items) item->set_visible(visible);
    }
    inline void projectiles_set_visible(bool visible){
        for(auto projectile : v_projectiles){ projectile->set_visible(visible);}
    }

    void set_all_visible(bool visible);
    void update();

    bn::random& randomizer;
    
    Graphics_Manager graphicsManager;

    Player cat;
    Stairs stairs;
    Healthbar healthbar;
    Fog<MAX_ROOMS> fog;

    NPCs_vector_t v_npcs;
    enemies_vector_t v_enemies;
    items_vector_t v_scene_items;
    projectiles_vector_t v_projectiles;
};

}


#endif
