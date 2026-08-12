#include "jv_graphics_manager.h"

namespace jv{
Graphics_Manager::Graphics_Manager(){
    for(int i = 0; i < MAX_ACTORS; i++){
        us_graphic_indexes.insert(i);
    }
}

Actor::Graphics& Graphics_Manager::operator[](Graphics_Manager::key_t key){
    auto graphic = um_graphics.find(key);
    //BN_ASSERT(graphic != um_graphics.end(), "Tried to get nonexistent sprite.");
    return graphic->second;
}

void Graphics_Manager::create_sprite(Graphics_Manager::key_t& actor_key){
    Graphics_Manager::key_t key = *us_graphic_indexes.begin();
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

void Graphics_Manager::update(){
    for(auto it = um_graphics.begin(); it != um_graphics.end(); it++){
        if(!it->second.animation.done())it->second.animation.update();
    }
}
}