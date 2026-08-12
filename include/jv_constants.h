#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

#include "bn_fixed.h"
#include "bn_array.h"
#include "bn_string.h"

/* Glossary                         *
 * Tile: 8x8 pixel background tile  *
 * Block: 4x4 tile array            */

#define DEV_ENABLED true    // Turns all dev exclusive functionality on/off

#define FADE_IN true
#define FADE_OUT false

namespace {
    constexpr bn::string_view Version = "0.1.2";
    constexpr uint8_t WTILES_COUNT = 76;        // Number of Walkable Tiles

    constexpr uint8_t NPCS_COUNT = 2;
    constexpr uint8_t ACTOR_COUNT = 7;
    constexpr uint8_t MAX_ROOM_ROWS = 5;
    constexpr uint8_t MAX_ROOM_COLUMNS = 5;
    constexpr uint8_t MAX_ROOMS = MAX_ROOM_ROWS*MAX_ROOM_COLUMNS;
    constexpr uint16_t MAX_ENEMIES = MAX_ROOMS*3;
    
    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2
}

namespace jv::animation{
    constexpr int MAX_FRAMES = 8;
    enum class Id {Walk, Attack, Die, Idle};
    
    constexpr bn::array<uint16_t, 4> Walk_do[2] =   {{0, 1, 0, 2}, {0, 1, 2, 3}};
    constexpr bn::array<uint16_t, 4> Walk_ho[2] =   {{3, 4, 3, 5}, {4, 5, 6, 7}};
    constexpr bn::array<uint16_t, 4> Walk_up[2] =   {{6, 7, 6, 8}, {8, 9,10,11}};
    constexpr bn::array<uint16_t, 4> Attack_do[2] = {{0, 9,10,11}, {0, 9,10,11}};
    constexpr bn::array<uint16_t, 4> Attack_ho[2] = {{3,12,13,14}, {3,12,13,14}};
    constexpr bn::array<uint16_t, 4> Attack_up[2] = {{6,15,16,17}, {6,15,16,17}};
    
    constexpr bn::array<uint16_t, 4> idle[2] = {{18, 19, 18, 20}, {12, 13, 14, 15}};     // Idle down
    constexpr bn::array<uint16_t, 5> hurt = {21, 22, 21, 22, 22}; // Hurt down
}

namespace fadespeed{
    constexpr uint8_t VERYFAST = 15;  // Quarter of a second fade
    constexpr uint8_t FAST = 30;      // Half a second fade
    constexpr uint8_t MEDIUM = 60;    // One second fade
    constexpr uint8_t SLOW = 120;     // Two seconds fade
}

#endif