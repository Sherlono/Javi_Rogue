#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_blending_actions.h"

#include "jv_environment.h"

#if LOGS_ENABLED
    #include "bn_log.h"
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
        BN_LOG("Stack iwram: ", bn::memory::used_stack_iwram(), " Static iwram: ", bn::memory::used_static_iwram());
        BN_LOG("Alloc ewram: ", bn::memory::used_alloc_ewram(), " Static ewram: ", bn::memory::used_static_ewram());
        //BN_LOG("Rom: ", bn::memory::used_rom());
        BN_LOG("Sprites count: ", bn::sprites::used_items_count(), " Backgrounds count: ", bn::bgs::used_items_count());
    #endif
}

void random_coords(auto& points_out, game_map& map, bn::random& randomizer){
    bn::point* valid_points = nullptr;
    int width = (map.x()-2)>>2, height = (map.y()-3)>>2;
    int pointCount = 0, current_size = 0;
    int index[4] = {0, 0, 0, 0};

    // Finding coordinates with floor in them
    for(int y = 2; y < height; y++){
        for(int x = 1; x < width; x++){
            bool w_check = true; // Walkable check
            index[0] = x*4 + y*4*map.x();
            index[1] = x*4 + 3 + y*4*map.x();
            index[2] = x*4 + (y*4 + 3)*map.x();
            index[3] = x*4 + 3 + (y*4 + 3)*map.x();

            for(int i = 0; i < 4; i++){
                int ind = index[i];
                w_check = w_check && (map[ind] > 0 && map[ind] < WT_COUNT);
            }
            
            if(w_check){
                // Simple dinamically growing array code. Double size when capped
                pointCount++;
                if(valid_points == nullptr){
                    valid_points = new bn::point[1];
                    valid_points[0] = bn::point(16 + 32*x, 16 + 32*y);
                    current_size = 1;
                }else if(pointCount >= current_size){
                    current_size = current_size*2;
                    bn::point* temp = new bn::point[current_size];
                    for(int i = 0; i < pointCount - 1; i++){
                        temp[i] = valid_points[i];
                    }
                    temp[pointCount - 1] = bn::point(16 + 32*x, 16 + 32*y);

                    delete[] valid_points;
                    valid_points = temp;
                }else{
                    valid_points[pointCount - 1] = bn::point(16 + 32*x, 16 + 32*y);
                }
            }
        }
    }
    // Storing random valid coordinates
    for(int i = 0; i < points_out.max_size(); i++){
        int block = randomizer.get_int(0, pointCount);
        points_out.push_back(bn::point(valid_points[block].x(), valid_points[block].y()));
    }
    delete[] valid_points;
}

inline void fade(const bool fadeIn, const unsigned char speed = fadespeed::MEDIUM){
    if(fadeIn){
        bn::fixed progress = 1.0;
        for(int i = 0; progress >= 0; i++){
            progress = 1 - bn::fixed(i)/speed;
            bn::blending::set_fade_alpha(bn::max(progress, bn::fixed(0)));
            bn::core::update();
        }
    }else{
        bn::fixed progress = 0.0;
        for(int i = 0; progress <= 1; i++){
            progress = bn::fixed(i)/speed;
            bn::blending::set_fade_alpha(bn::min(progress, bn::fixed(1)));
            bn::core::update();
        }
    }
}

}

#endif