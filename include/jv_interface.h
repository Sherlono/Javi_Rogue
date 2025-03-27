#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_keypad.h"

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

template <typename PointsVector, typename DataArray>
void random_coords(PointsVector& points_out, bn::random& randomizer, DataArray& map, const int width, const int height){
    bn::point* valid_points = nullptr;
    int pointCount = 0, current_size = 0;
    // Finding coordinates with floor in them
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int index = x + y*width;
            if(map[index]){
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

}

#endif