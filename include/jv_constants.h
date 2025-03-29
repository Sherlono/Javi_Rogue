#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H


namespace {
    typedef const unsigned char cuchar_t;
    typedef unsigned char uchar_t;
    
    constexpr cuchar_t F_COUNT = 20;    // Number of floor blocks

    constexpr bn::fixed SQRTTWODTWO = 0.707107;        // sqrt(2)/2
    constexpr bn::fixed SQRTTWODTWOMONE = 0.292893;    // (sqrt(2)/2) - 1
}

#endif