#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

/**
 *  Glossary
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

#define LOGS_ENABLED true   // Turns all BN_LOG call functionality on/off
#define DEV_ENABLED true    // Turns all dev exclusive functionality on/off

#define FADE_IN true
#define FADE_OUT false

namespace {
    constexpr uint8_t WTILES_COUNT = 46;                            // Number of Walkable Tiles
    constexpr uint8_t NWTILES_COUNT = 52;                           // Number of Not Walkable Tiles
    constexpr uint8_t TILES_TOTAL = WTILES_COUNT + NWTILES_COUNT;   // Number of Total Tiles
    constexpr uint8_t ROOM_PREFAB_COUNT = 8;                        // Number of Room Prefabs

    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2

    constexpr bn::fixed quarter_circle[24] =
        {0.00087, 0.00348, 0.00784, 0.014, 0.022, 0.03175, 0.0435, 0.0572, 0.073, 0.091, 0.11122, 0.134,
         0.16, 0.18777, 0.22, 0.25464, 0.3, 0.33856, 0.39, 0.44723, 0.51588, 0.6, 0.71435, 1.0};

    constexpr int MAX_ENEMIES = 15;
    constexpr int MAX_ROOM_ROWS = 6;
    constexpr int MAX_ROOM_COLUMNS = 6;
    constexpr int MAX_ROOMS = MAX_ROOM_ROWS*MAX_ROOM_COLUMNS;
}

namespace jv::animation{
    constexpr int MAX_FRAMES = 8;
    enum Id {Walk, Attack};
    
    constexpr bn::array<uint16_t, 4> Walk_up = {6, 7, 6, 8};
    constexpr bn::array<uint16_t, 4> Walk_ho = {3, 4, 3, 5};
    constexpr bn::array<uint16_t, 4> Walk_do = {0, 1, 0, 2};
    constexpr bn::array<uint16_t, 4> Attack_up = {6, 15, 16, 17};      // Walk/Attack up
    constexpr bn::array<uint16_t, 4> Attack_ho = {3, 12, 13, 14};      // Walk/Attack horizontal
    constexpr bn::array<uint16_t, 4> Attack_do = {0, 9, 10, 11};      // Walk/Attack down
    
    constexpr bn::array<uint16_t, 4> idle = {18, 19, 18, 20};  // Idle down
    constexpr bn::array<uint16_t, 5> hurt = {21, 22, 21, 22, 22};  // Hurt down
}

#endif