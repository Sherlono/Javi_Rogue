#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

namespace {
    typedef const unsigned char cuchar_t;
    typedef unsigned char uchar_t;
    
    cuchar_t W_COUNT = 28, F_COUNT = 50;  // The count for each different block type. Crucial to keep up to date!!!
    constexpr cuchar_t  BLOCK_TYPE_COUNT = W_COUNT + F_COUNT;
    constexpr cuchar_t MAX_PARA = 255;  // Not arbitrary
    constexpr cuchar_t MAX_BLOCKS = 77;  // Not arbitrary
}

#endif