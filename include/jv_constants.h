#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

namespace {
    typedef const unsigned char cuchar_t;
    typedef const char cchar_t;
    
    cuchar_t W_COUNT = 28, F_COUNT = 50;  // The count for each different block type. Crucial to keep up to date!!!
    constexpr cuchar_t  BLOCK_TYPE_COUNT = W_COUNT + F_COUNT;
    constexpr int MAX_BLOCKS = 128;  // Not arbitrary
}

#endif