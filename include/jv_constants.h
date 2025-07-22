#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

/**
 *  Glossary
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

#ifndef LOGS_ENABLED
    #define LOGS_ENABLED true   // Turns all BN_LOG call functionality on/off
#endif

#ifndef DEV_ENABLED
    #define DEV_ENABLED true    // Turns all dev exclusive functionality on/off
#endif

namespace {
    constexpr uint8_t WTILES_COUNT = 46;                            // Number of Walkable Tiles
    constexpr uint8_t NWTILES_COUNT = 52;                           // Number of Not Walkable Tiles
    constexpr uint8_t TILES_TOTAL = WTILES_COUNT + NWTILES_COUNT;   // Number of Total Tiles
    constexpr uint8_t ROOM_PREFAB_COUNT = 8;                        // Number of Room Prefabs

    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2

    constexpr bn::fixed quarter_circle[24] =
        {0.00087, 0.00348, 0.00784, 0.014, 0.022, 0.03175, 0.0435, 0.0572, 0.073, 0.091, 0.11122, 0.134,
         0.16, 0.18777, 0.22, 0.25464, 0.3, 0.33856, 0.39, 0.44723, 0.51588, 0.6, 0.71435, 1.0};

    constexpr int MAX_FRAMES = 8;
    constexpr int MAX_ENEMIES = 15;
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

#endif