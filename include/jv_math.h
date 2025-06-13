#ifndef JV_MATH_H
#define JV_MATH_H

#include "bn_math.h"
#include "bn_point.h"
#include "bn_fixed.h"

namespace jv{
 // "Bitwise And Modulus". Accurately returns the modulo (as long as d is a power of two) faster.
inline int bamod(int n, int d){
    return n & (d-1);
}

inline bn::fixed lerp(bn::fixed a, int b, bn::fixed t) {
    return a + (b-a) * t;
}

inline const bn::fixed_point normalize(const bn::fixed_point vector){
    bn::fixed length = bn::sqrt(vector.x()*vector.x()+vector.y()*vector.y());
    if(length != 0){
        return bn::fixed_point(vector.x()/length, vector.y()/length);
    }else{
        return bn::fixed_point(0, 0);
    }
}

}

#endif