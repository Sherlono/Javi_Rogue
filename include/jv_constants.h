#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

/* Glossary
 * uchar: Unsigned char
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

namespace {
    typedef unsigned char uchar_t;
    typedef const unsigned char cuchar_t;
    
    constexpr cuchar_t F_COUNT = 21,                // Number of Floor Blocks
                       W_COUNT = 36 - F_COUNT + 1,  // Number of Wall Blocks
                       B_COUNT = F_COUNT + W_COUNT, // Total Blocks
                       WT_COUNT = 45;               // Number of Walkable Tiles

    constexpr bn::fixed SQRTTWODTWO = 0.707107;        // sqrt(2)/2
    constexpr bn::fixed SQRTTWODTWOMONE = 0.292893;    // (sqrt(2)/2) - 1
}

#endif