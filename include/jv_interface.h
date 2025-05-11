#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_assert.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"

#include "jv_environment.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_bgs.h"
    #include "bn_sprites.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

namespace jv{
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
    #endif
}
inline void Log_resources(){
    #if LOGS_ENABLED
        BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());
        BN_LOG("Stack iwram: ", bn::memory::used_stack_iwram(), " Static iwram: ", bn::memory::used_static_iwram());
        bn::memory::log_alloc_ewram_status();
        //BN_LOG("Rom: ", bn::memory::used_rom());
    #endif
}

[[nodiscard]] inline bn::point pop_point(bn::point* points, int& size, const int index){
    BN_ASSERT(size > 0, "Invalid size: ", size);
    BN_ASSERT(index < size, "Invalid index: ", index);
    bn::point out = bn::point(points[index].x(), points[index].y());

    points[index] = points[size-1];
    size = size - 1;
    return out;
}

void random_coords(auto& points_out, game_map& map, bn::random& randomizer){
    bn::point* valid_points = nullptr;
    int pointCount = 0;

    {
        int current_size = 0;
        int width = (map.x() - 2)>>2, height = (map.y() - 3)>>2;
        int tileIndex[4] = {0, 0, 0, 0};

        // Finding coordinates with floor in them
        for(int y = 2; y < height; y++){
            for(int x = 1; x < width; x++){
                bool w_check = true; // Walkable check
                tileIndex[0] = x*4 - 1 + (y*4 - 1 )*map.x();
                tileIndex[1] = x*4 + 2 + (y*4 - 1 )*map.x();
                tileIndex[2] = x*4 - 1 + (y*4 + 2)*map.x();
                tileIndex[3] = x*4 + 2 + (y*4 + 2)*map.x();

                for(int i = 0; i < 4; i++){
                    w_check = w_check && (map[tileIndex[i]] > 0 && map[tileIndex[i]] < WTILES_COUNT);
                }
                
                if(w_check){
                    // Simple dinamically growing array code. Double size when capped
                    pointCount++;
                    if(valid_points == nullptr){
                        valid_points = new bn::point[1];
                        valid_points[0] = bn::point(32*x, 32*y);
                        current_size = 1;
                    }else if(pointCount >= current_size){
                        current_size = current_size*2;
                        bn::point* temp = new bn::point[current_size];
                        for(int i = 0; i < pointCount - 1; i++){
                            temp[i] = valid_points[i];
                        }
                        temp[pointCount - 1] = bn::point(32*x, 32*y);

                        delete[] valid_points;
                        valid_points = temp;
                    }else{
                        valid_points[pointCount - 1] = bn::point(32*x, 32*y);
                    }
                }
            }
        }
    }
    // Storing random valid coordinates
    for(int i = 0; i < points_out.max_size(); i++){
        int pointIndex = randomizer.get_int(0, pointCount);
        points_out.push_back(pop_point(valid_points, pointCount, pointIndex));
    }
    delete[] valid_points;
}

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

#endif