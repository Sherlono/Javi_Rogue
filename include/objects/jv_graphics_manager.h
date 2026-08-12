#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include "bn_unordered_set.h"
#include "bn_unordered_map.h"

#include "jv_actors.h"
#include "jv_constants.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

namespace jv{
constexpr uint8_t MAX_ACTORS = 2*2*2*2*2*2;

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
        #if DEV_ENABLED
        BN_LOG("Front key: ", f_key);
        #endif
        return f_key;
    }
    // Returns wether a graphics instance exists with a respective key
    inline bool find(key_t key){
        return um_graphics.find(key) != um_graphics.end();
    }
    inline bool is_available(key_t key){
        return us_graphic_indexes.find(key) != us_graphic_indexes.end();
    }
    
    Actor::Graphics & operator[](key_t key);

    // Returns the first available key in the set
    void create_sprite(key_t& actor_key);
    void erase_sprite(key_t& actor_key);
    void set_all_visible(bool visible){
        for(auto it = um_graphics.begin(); it != um_graphics.end(); it++) it->second.set_visible(visible);
    }
    void update();

private:
    using graphics_uset_t = bn::unordered_set<key_t, MAX_ACTORS>;
    using graphics_umap_t = bn::unordered_map<key_t, Actor::Graphics, MAX_ACTORS>;

    graphics_uset_t us_graphic_indexes;
    graphics_umap_t um_graphics;
};
}

#endif