#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

namespace {
    typedef const unsigned char cuchar_t;
    typedef const char cchar_t;
    
    cuchar_t W1_COUNT = 23, W2_COUNT = 5, F_COUNT = 50;  // The count for each different block type. Crucial to keep up to date!!!
    constexpr cuchar_t W1W2_COUNT = W1_COUNT + W2_COUNT, BLOCK_TYPE_COUNT = W1W2_COUNT + F_COUNT;
    constexpr int MAX_BLOCKS = 13 * 9;  // Not arbitrary
}

#endif