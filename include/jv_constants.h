#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

/**
 *  Glossary
 * uchar: Unsigned char
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

namespace {
    typedef unsigned char uchar_t;
    typedef const unsigned char cuchar_t;
    
    constexpr uchar_t F_COUNT = 22,                // Number of Floor Blocks [0,21]
                       W_COUNT = 43 - F_COUNT + 1,  // Number of Wall Blocks [22,43]
                       B_COUNT = F_COUNT + W_COUNT, // Total Blocks
                       WT_COUNT = 46;               // Number of Walkable Tiles

    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2
}

namespace jv::frames{
    constexpr uchar_t w_up[4] = {6, 7, 6, 8};      // Walk up frames
    constexpr uchar_t w_ho[4] = {3, 4, 3, 5};      // Walk horizontal frames
    constexpr uchar_t w_do[4] = {0, 1, 0, 2};      // Walk down frames

    constexpr uchar_t a_up[4] = {6, 15, 16, 17};   // Attack up frames
    constexpr uchar_t a_ho[4] = {3, 12, 13, 14};   // Attack horizontal frames
    constexpr uchar_t a_do[4] = {0, 9, 10, 11};    // Attack down frames
    
    constexpr uchar_t idle[4] = {18, 19, 18, 20};  // Idle frames
    constexpr uchar_t hurt[4] = {22, 23, 22, 23};  // Hurt down frames
}

namespace jv::fadespeed{
    constexpr unsigned char FAST = 30;
    constexpr unsigned char MEDIUM = 60;
    constexpr unsigned char SLOW = 120;
}

#endif