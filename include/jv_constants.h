#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

#include "bn_string.h"

/* Glossary                         *
 * Tile: 8x8 pixel background tile  *
 * Block: 4x4 tile array            */

#define DEV_ENABLED true    // Turns all dev exclusive functionality on/off

#define FADE_IN true
#define FADE_OUT false

namespace {
    constexpr bn::string_view Version = "0.1.1";
    constexpr uint8_t WTILES_COUNT = 60;        // Number of Walkable Tiles
    constexpr uint8_t ROOM_PREFAB_COUNT = 9;    // Number of Room Prefabs

    constexpr uint8_t MAX_ENEMIES = 15;
    constexpr uint8_t MAX_ROOM_ROWS = 6;
    constexpr uint8_t MAX_ROOM_COLUMNS = 6;
    constexpr uint8_t MAX_ROOMS = MAX_ROOM_ROWS*MAX_ROOM_COLUMNS;
    
    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2
}

namespace jv::animation{
    constexpr int MAX_FRAMES = 8;
    enum Id {Walk, Attack, Die};
    
    constexpr bn::array<uint16_t, 4> Walk_up = {6, 7, 6, 8};
    constexpr bn::array<uint16_t, 4> Walk_ho = {3, 4, 3, 5};
    constexpr bn::array<uint16_t, 4> Walk_do = {0, 1, 0, 2};
    constexpr bn::array<uint16_t, 4> Attack_up = {6, 15, 16, 17};
    constexpr bn::array<uint16_t, 4> Attack_ho = {3, 12, 13, 14};
    constexpr bn::array<uint16_t, 4> Attack_do = {0, 9, 10, 11}; 
    
    constexpr bn::array<uint16_t, 4> idle = {18, 19, 18, 20};     // Idle down
    constexpr bn::array<uint16_t, 5> hurt = {21, 22, 21, 22, 22}; // Hurt down
}

#endif