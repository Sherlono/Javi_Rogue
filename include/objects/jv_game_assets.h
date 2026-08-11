#ifndef JV_GAME_ASSETS_H
#define JV_GAME_ASSETS_H

#include "bn_random.h"

#include "bn_log.h"
#include "bn_vector.h"
#include "bn_unordered_set.h"
#include "bn_unordered_map.h"

#include "jv_fog.h"
#include "jv_items.h"
#include "jv_actors.h"
#include "jv_player.h"
#include "jv_stairs.h"
#include "jv_projectile.h"
#include "jv_constants.h"
#include "jv_healthbar.h"

namespace jv{
class Graphics_Manager{
public:
    using key_t = uint8_t;

    Graphics_Manager();
    Graphics_Manager(Graphics_Manager &other) = delete;
    Graphics_Manager(Graphics_Manager &&other) = delete;
    
    [[nodiscard]] int size() {
        return um_graphics.size();
    }
    // Returns the next key to be used without altering the set
    inline key_t front_key(){
        key_t f_key = *us_graphic_indexes.begin();
        BN_LOG("Front key: ", f_key);
        return f_key;
    }
    // Returns wether a graphics instance exists with a respective key
    inline bool find(key_t key){
        return um_graphics.find(key) != um_graphics.end();
    }
    inline bool is_available(key_t key){
        return us_graphic_indexes.find(key) != us_graphic_indexes.end();
    }
    
    
    Actor::Graphics & operator[](key_t key){
        auto graphic = um_graphics.find(key);
        //BN_ASSERT(graphic != um_graphics.end(), "Tried to get nonexistent sprite.");
        return graphic->second;
    }

    // Returns the first available key in the set
    void create_sprite(key_t& actor_key);
    void erase_sprite(key_t& actor_key);
    
    void graphics_update();

//private:
    using graphics_uset_t = bn::unordered_set<uint8_t, MAX_ACTORS>;
    using graphics_umap_t = bn::unordered_map<uint8_t, Actor::Graphics, MAX_ACTORS>;

    graphics_uset_t us_graphic_indexes;
    graphics_umap_t um_graphics;
};

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
    void graphics_update();

    inline void npcs_set_visible(bool visible){
        for(auto npc : v_npcs) npc->set_visible(visible);
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
