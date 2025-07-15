#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_assert.h"
#include "bn_colors.h"
#include "bn_memory.h"
#include "bn_sprite_ptr.h"
#include "bn_bg_palettes.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"

#include "jv_global.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_bgs.h"
    #include "bn_sprites.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

class game_map;
class Zone;

namespace jv{
class Player;
class tiled_bg;
struct stairs;
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

inline void Log_skipped_frames(){
    #if LOGS_ENABLED
        int skipped = bn::core::last_missed_frames();
        if(skipped != 0){
            BN_LOG("******************");
            BN_LOG("Frames skipped: ", skipped);
            BN_LOG("******************");
        }
        BN_LOG("CPU usage: ", bn::core::last_cpu_usage());
    #endif
}

inline void Log_resources(){
    #if LOGS_ENABLED
        BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());
        BN_LOG("Used alloc ewram: ", bn::memory::used_alloc_ewram(), " Available alloc ewram: ", bn::memory::available_alloc_ewram());
        BN_LOG("Stack iwram: ", bn::memory::used_stack_iwram(), " Static iwram: ", bn::memory::used_static_iwram());
        //BN_LOG("Rom: ", bn::memory::used_rom());
    #endif
}

void Log_zone_layout(Zone& z);

void set_hide_all(jv::healthbar& healthbar, jv::stairs& stairs, bn::regular_bg_ptr& background, jv::tiled_bg& Fortress, bn::ivector<bn::sprite_ptr>& txt, bool hide);

[[nodiscard]] inline bn::point pop_point(bn::point* points, int& size, const int index){
    BN_ASSERT(size > 0, "Invalid size: ", size);
    BN_ASSERT(index < size, "Invalid index: ", index);
    bn::point out = bn::point(points[index].x(), points[index].y());

    points[index] = points[size-1];
    size = size - 1;
    return out;
}

void random_coords(bn::ivector<bn::point>& points_out);

inline void fade(const bool fadeIn, const unsigned char speed = fadespeed::MEDIUM){
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
        progress = 0.0;
        bn::fixed min;
        for(int i = 0; progress <= 1; i++){
            progress = bn::fixed(i)/speed;
            min = bn::min(progress, bn::fixed(1));
            bn::sprite_palettes::set_fade(black, min);
            bn::bg_palettes::set_fade(black, min);
            bn::core::update();
        }
    }
}

}
}

#endif