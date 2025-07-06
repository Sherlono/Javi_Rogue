#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

/**
 *  Glossary
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

#ifndef LOGS_ENABLED
    #define LOGS_ENABLED true      // Turns all BN_LOG call functionality on/off
#endif

#ifndef DEV_ENABLED
    #define DEV_ENABLED true       // Turns all dev exclusive functionality on/off
#endif

namespace {
    constexpr uint8_t FLOOR_COUNT = 8;                              // Number of Floor Blocks [0,21]
    constexpr uint8_t WALL_COUNT = 30 - FLOOR_COUNT;                // Number of Wall Blocks [22,43]
    constexpr uint8_t BLOCK_TOTAL = FLOOR_COUNT + WALL_COUNT + 1;   // Total Blocks
    constexpr uint8_t WTILES_COUNT = 46;                            // Number of Walkable Tiles
    constexpr uint8_t NWTILES_COUNT = 52;                           // Number of Not Walkable Tiles
    constexpr uint8_t TILES_TOTAL = WTILES_COUNT + NWTILES_COUNT;   // Number of Total Tiles
    constexpr uint8_t ROOM_PREFAB_COUNT = 8;                        // Number of Room Prefabs

    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2

    constexpr bn::fixed quarter_circle[24] =
        {0.00087, 0.00348, 0.00784, 0.014, 0.022, 0.03175, 0.0435, 0.0572, 0.073, 0.091, 0.11122, 0.134,
         0.16, 0.18777, 0.22, 0.25464, 0.3, 0.33856, 0.39, 0.44723, 0.51588, 0.6, 0.71435, 1.0};

    constexpr int MAX_FRAMES = 8;
    constexpr int MAX_ENEMIES = 10;
}

namespace jv::fadespeed{
    constexpr uint8_t VERYFAST = 15;  // Quarter of a second fade
    constexpr uint8_t FAST = 30;      // Half a second fade
    constexpr uint8_t MEDIUM = 60;    // One second fade
    constexpr uint8_t SLOW = 120;     // Two seconds fade
}

namespace jv::frames{
    constexpr uint8_t Walk = 0;
    constexpr uint8_t Attack = 1;
    
    constexpr uint16_t WalkAttack_up[2][4] = {{6, 7, 6, 8}, {6, 15, 16, 17}};      // Walk/Attack up
    constexpr uint16_t WalkAttack_ho[2][4] = {{3, 4, 3, 5}, {3, 12, 13, 14}};      // Walk/Attack horizontal
    constexpr uint16_t WalkAttack_do[2][4] = {{0, 1, 0, 2}, {0, 9, 10, 11} };      // Walk/Attack down
    
    constexpr uint16_t idle[4] = {18, 19, 18, 20};  // Idle down
    constexpr uint16_t hurt[4] = {22, 23, 22, 23};  // Hurt down
}

namespace jv::blocks{
    constexpr uint8_t data[BLOCK_TOTAL][16] = {
        //block 0
        {0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0}, 
        //block 1
        {1, 1, 2, 1, 
        31, 30, 31, 31, 
        1, 1, 2, 1, 
        31, 30, 31, 31}, 
        //block 2
        {5, 6, 7, 8, 
        31, 30, 31, 31, 
        128, 128, 128, 129, 
        31, 30, 31, 31}, 
        //block 3
        {8, 1, 2, 1, 
        31, 30, 31, 31, 
        1, 1, 2, 1, 
        31, 30, 31, 31}, 
        //block 4
        {5, 6, 7, 8, 
        31, 30, 31, 31, 
        9, 10, 3, 4, 
        32, 33, 26, 27}, 
        //block 5
        {1, 1, 2, 1, 
        31, 30, 31, 31, 
        9, 10, 3, 4, 
        32, 33, 26, 27}, 
        //block 6
        {1, 1, 2, 1, 
        28, 29, 31, 31, 
        14, 15, 3, 4, 
        75, 75, 36, 37}, 
        //block 7
        {14, 15, 3, 4, 
        75, 75, 36, 37, 
        89, 90, 75, 75, 
        0, 0, 89, 90}, 
        //block 8
        {73, 74, 16, 17, 
        38, 39, 24, 25, 
        11, 12, 2, 1, 
        31, 30, 31, 31}, 
        //block 9
        {50, 51, 178, 177, 
        48, 49, 176, 175, 
        50, 51, 178, 177, 
        52, 53, 87, 88}, 
        //block 10
        {46, 47, 174, 173, 
        48, 49, 176, 175, 
        50, 51, 178, 177, 
        48, 49, 176, 175}, 
        //block 11
        {69, 70, 63, 64, 
        65, 66, 71, 72, 
        73, 74, 16, 17, 
        38, 39, 24, 25}, 
        //block 12
        {0, 0, 54, 80, 
        57, 58, 59, 60, 
        61, 62, 63, 64, 
        65, 66, 67, 68}, 
        //block 13
        {0, 0, 54, 56, 
        57, 58, 59, 60, 
        61, 62, 63, 64, 
        65, 66, 67, 68}, 
        //block 14
        {50, 51, 178, 177, 
        48, 49, 176, 175, 
        50, 51, 178, 177, 
        48, 49, 176, 175}, 
        //block 15
        {0, 0, 0, 0, 
        0, 0, 0, 0, 
        46, 47, 174, 173, 
        48, 49, 176, 175}, 
        //block 16
        {61, 62, 63, 64, 
        65, 66, 67, 68, 
        69, 70, 63, 64, 
        65, 66, 71, 72}, 
        //block 17
        {0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 54, 80, 
        57, 58, 59, 60}, 
        //block 18
        {0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 54, 56, 
        57, 58, 59, 60}, 
        //block 19
        {0, 0, 79, 80, 
        0, 0, 81, 82, 
        0, 0, 83, 84, 
        0, 0, 81, 82}, 
        //block 20
        {0, 0, 81, 82, 
        0, 0, 83, 84, 
        0, 0, 81, 82, 
        0, 0, 83, 84}, 
        //block 21
        {0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 79, 80, 
        0, 0, 83, 84}, 
        //block 22
        {0, 0, 81, 82, 
        0, 0, 85, 86, 
        0, 0, 0, 0, 
        0, 0, 0, 0}, 
        //block 23
        {209, 208, 0, 0, 
        211, 210, 0, 0, 
        46, 47, 174, 173, 
        48, 49, 176, 175}, 
        //block 24
        {209, 208, 81, 82, 
        211, 210, 83, 84, 
        46, 47, 174, 173, 
        48, 49, 176, 175}, 
        //block 25
        {75, 75, 75, 75, 
        75, 75, 75, 75, 
        0, 0, 0, 0, 
        0, 0, 0, 0}, 
        //block 26
        {89, 90, 75, 75, 
        0, 0, 89, 90, 
        0, 0, 0, 0, 
        0, 0, 0, 0}, 
        //block 27
        {75, 75, 77, 82, 
        75, 75, 78, 84, 
        0, 0, 81, 82, 
        0, 0, 83, 84}, 
        //block 28
        {209, 208, 81, 82, 
        211, 210, 83, 84, 
        209, 208, 81, 82, 
        211, 210, 83, 84}, 
        //block 29
        {1, 1, 2, 1, 
        31, 30, 31, 31, 
        46, 47, 174, 173, 
        48, 49, 176, 175}, 
        //block 30
        {209, 208, 81, 82, 
        211, 210, 83, 84, 
        0, 0, 0, 0, 
        0, 0, 0, 0}
    };
}

#endif