#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_colors.h"
#include "bn_memory.h"
#include "bn_sprite_ptr.h"
#include "bn_bg_palettes.h"
#include "bn_music_actions.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"

#include "jv_global.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_bgs.h"
    #include "bn_string.h"
    #include "bn_sprites.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

class GameMap;
class Zone;

namespace jv{
class Player;
class tiled_bg;
struct healthbar;

namespace fadespeed{
    constexpr uint8_t VERYFAST = 15;  // Quarter of a second fade
    constexpr uint8_t FAST = 30;      // Half a second fade
    constexpr uint8_t MEDIUM = 60;    // One second fade
    constexpr uint8_t SLOW = 120;     // Two seconds fade
}

namespace Interface{
inline void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){ bn::core::reset();}
}

inline void fade(const bool fadeIn, const unsigned char speed, const bool fademusic = false){
    bn::fixed progress;
    bn::color black = bn::colors::black;

    if(fadeIn){
        progress = 1.0;
        bn::fixed max;
        for(int i = 0; progress >= 0; i++){
            progress = 1 - bn::fixed(i)/speed;
            max = bn::max(progress, bn::fixed(0));
            bn::sprite_palettes::set_fade(black, max);
            bn::bg_palettes::set_fade(black, max);
            bn::core::update();
        }
    }else{
        bn::fixed volume, volume_decrement;
        if(fademusic && bn::music::playing()){
            volume_decrement = bn::music::volume()/speed;
        }
        progress = 0.0;
        bn::fixed min;

        for(int i = 0; progress <= 1; i++){
            progress = bn::fixed(i)/speed;
            min = bn::min(progress, bn::fixed(1));
            bn::sprite_palettes::set_fade(black, min);
            bn::bg_palettes::set_fade(black, min);
            if(fademusic && bn::music::playing()){
                volume = bn::music::volume();
                bn::music::set_volume(bn::max(volume - volume_decrement, bn::fixed(0)));
            }
            bn::core::update();
        }
    }
}

inline void Log_skipped_frames(){
    int skipped = bn::core::last_missed_frames();
    if(skipped != 0){
        BN_LOG("******************");
        BN_LOG("Frames skipped: ", skipped);
        BN_LOG("******************");
    }
}

inline void Log_resources(){
    BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());
    BN_LOG("Used alloc ewram: ", bn::memory::used_alloc_ewram(), " Available alloc ewram: ", bn::memory::available_alloc_ewram());
    BN_LOG("Stack iwram: ", bn::memory::used_stack_iwram(), " Static iwram: ", bn::memory::used_static_iwram());
    //BN_LOG("Rom: ", bn::memory::used_rom());
}

void Log_zone_layout(Zone& z);
void set_hide_all(jv::healthbar& healthbar, bn::regular_bg_ptr& background, jv::tiled_bg& Fortress, bn::ivector<bn::sprite_ptr>& txt, bool hide);

}
}

#endif