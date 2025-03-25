#ifndef JV_MATH_H
#define JV_MATH_H

#include "bn_point.h"
//#include "bn_rect.h"
#include "bn_fixed.h"

namespace jv{
 // Bitwise And Modulus. Accurately returns the modulo (as long as d is a power of two) faster.
inline int bamod(int n, int d){ return n & (d-1);}

}

#endif