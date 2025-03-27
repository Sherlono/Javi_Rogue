#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_random.h"

#include "jv_environment.h"

namespace jv{
inline void resetcombo(){
    if(bn::keypad::a_held() && bn::keypad::b_held() && bn::keypad::start_held() && bn::keypad::select_held()){ bn::core::reset();}
}

void Log_skipped_frames(){
    int skipped = bn::core::last_missed_frames();
    if(skipped != 0){
        BN_LOG("******************");
        BN_LOG("Frames skipped: ", skipped);
        BN_LOG("******************");
    }
}

template <typename PointsVector, typename DataArray>
void random_coords(PointsVector& points, bn::random& randomizer, DataArray& map, const int width, const int height){
    bn::point* blocked = new bn::point[width*height];
    int blockCount = 0;
    // Finding coordinates with floor in them
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int index = x + y*width;
            if(map[index]){
                blocked[blockCount] = bn::point(16 + 32*x, 16 + 32*y);
                blockCount++;
            }
        }
    }
    // Storing random valid coordinates
    for(int i = 0; i < points.max_size(); i++){
        int block = randomizer.get_int(0, blockCount);
        points.push_back(bn::point(blocked[block].x(), blocked[block].y()));
    }
    delete[] blocked;
}

}

#endif