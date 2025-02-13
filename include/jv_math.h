#ifndef JV_MATH_H
#define JV_MATH_H

#include "bn_point.h"
//#include "bn_rect.h"
#include "bn_fixed.h"

namespace jv{
inline int xproduct(bn::point a, bn::point b, bn::point t){  // Keep in mind the Y axis is inverted
    if (a.x() != b.x()){
        return (b.x() - a.x()) * (-t.y() + a.y()) + (b.y() - a.y()) * (t.x() - a.x());  // > 0 = above
    }else{
        return a.x() - t.x();   // > 0 = left
    }
}

}

#endif