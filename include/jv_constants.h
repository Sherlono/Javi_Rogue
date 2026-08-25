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
    constexpr bn::string_view Version = "0.1.3";
    constexpr uint8_t WTILES_COUNT = 76;        // Number of Walkable Tiles

    constexpr uint8_t NPCS_COUNT = 2;
    constexpr uint8_t ACTOR_COUNT = 8;
    constexpr uint8_t MAX_ROOM_ROWS = 6;
    constexpr uint8_t MAX_ROOM_COLUMNS = 6;
    constexpr uint8_t MAX_ROOMS = MAX_ROOM_ROWS*MAX_ROOM_COLUMNS;
    constexpr uint16_t MAX_ENEMIES = MAX_ROOMS*3;
    
    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2
}

namespace fadespeed{
    constexpr uint8_t VERYFAST = 15;  // Quarter of a second fade
    constexpr uint8_t FAST = 30;      // Half a second fade
    constexpr uint8_t MEDIUM = 60;    // One second fade
    constexpr uint8_t SLOW = 120;     // Two seconds fade
}

#endif