#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_log.h"
#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_blending_actions.h"

#include "jv_environment.h"

namespace jv{
inline void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){ bn::core::reset();}
}

inline void Log_skipped_frames(){
    int skipped = bn::core::last_missed_frames();
    if(skipped != 0){
        BN_LOG("******************");
        BN_LOG("Frames skipped: ", skipped);
        BN_LOG("******************");
    }
}

void random_coords(auto& points_out, game_map& map, bn::random& randomizer){
    bn::point* valid_points = nullptr;
    int width = (map.x()-2) / 4, height = (map.y()-3) / 4;
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

inline void set_blending_enabled_bulk(bn::vector<bn::sprite_ptr, 128> v_sprts, bn::vector<bn::regular_bg_ptr, 4> v_bgs, bool Blend){
    for(bn::sprite_ptr s : v_sprts){
        s.set_blending_enabled(Blend);
    }
    for(bn::regular_bg_ptr b : v_bgs){
        b.set_blending_enabled(Blend);
    }

}

inline void fade(bn::vector<bn::sprite_ptr, 128> v_sprts, bn::vector<bn::regular_bg_ptr, 4> v_bgs, bool fadeIn){
    if(fadeIn){
        jv::set_blending_enabled_bulk(v_sprts, v_bgs, true);
        for(int i = 0; (1 - bn::fixed(i)/60) >= 0; i++){
            bn::blending::set_fade_alpha(bn::max(1 - bn::fixed(i)/60, bn::fixed(0)));
            bn::core::update();
        }
        jv::set_blending_enabled_bulk(v_sprts, v_bgs, false);
    }else{
        jv::set_blending_enabled_bulk(v_sprts, v_bgs, true);
        for(int i = 0; bn::fixed(i)/60 <= 1; i++){
            bn::blending::set_fade_alpha(bn::min(bn::fixed(i)/60, bn::fixed(1)));
            bn::core::update();
        }
        jv::set_blending_enabled_bulk(v_sprts, v_bgs, false);
    }
}

}

#endif