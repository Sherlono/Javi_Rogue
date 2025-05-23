#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

#include "bn_span.h"

/**
 *  Glossary
 * Tile: 8x8 pixel background tile
 * Block: 4x4 tile array
 */

#ifndef LOGS_ENABLED
    #define LOGS_ENABLED false       // Turns all BN_LOG call functionality off
#endif

namespace {
    constexpr uint8_t FLOOR_COUNT = 22;                          // Number of Floor Blocks [0,21]
    constexpr uint8_t WALL_COUNT = 45 - FLOOR_COUNT + 1;         // Number of Wall Blocks [22,43]
    constexpr uint8_t BLOCK_COUNT = FLOOR_COUNT + WALL_COUNT;    // Total Blocks
    constexpr uint8_t WTILES_COUNT = 46;                            // Number of Walkable Tiles
    constexpr uint8_t NWTILES_COUNT = 52;                           // Number of Not Walkable Tiles
    constexpr uint8_t TILES_COUNT = WTILES_COUNT + NWTILES_COUNT;   // Number of Total Tiles
    constexpr uint8_t ROOM_COUNT = 8;                               // Number of Room Prefabs

    constexpr bn::fixed ONEMSQRTTWODTWO = 0.292893;    // 1 - sqrt(2)/2

    constexpr bn::fixed quarter_circle[24] = {0.00087, 0.00348, 0.00784, 0.014, 0.022, 0.03175, 0.0435, 0.0572, 0.073, 0.091, 0.11122, 0.134,
                                              0.16, 0.18777, 0.22, 0.25464, 0.3, 0.33856, 0.39, 0.44723, 0.51588, 0.6, 0.71435, 1.0};

    constexpr int MAX_FRAMES = 8;
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
    
    constexpr uint16_t WalkAttack_up[2][4] = {{6, 7, 6, 8}, {6, 15, 16, 17}};      // Walk up frames
    constexpr uint16_t WalkAttack_ho[2][4] = {{3, 4, 3, 5}, {3, 12, 13, 14}};      // Walk horizontal frames
    constexpr uint16_t WalkAttack_do[2][4] = {{0, 1, 0, 2}, {0, 9, 10, 11} };      // Walk down frames
    
    constexpr uint16_t idle[4] = {18, 19, 18, 20};  // Idle frames
    constexpr uint16_t hurt[4] = {22, 23, 22, 23};  // Hurt down frames
}

namespace jv::blocks{
    constexpr uint8_t block_array[BLOCK_COUNT][16] = {
        // block 0
            {0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0},
        // block 1
            {1, 1, 2, 21, 
            22, 23, 22, 34, 
            1, 1, 2, 21, 
            22, 23, 22, 34}, 
        // block 2
            { 5, 6, 7, 8, 
            24, 25, 24, 24, 
            21, 21, 20, 21, 
            34, 33, 34, 34},
        // block 3
            { 5, 6, 7, 8, 
            40, 25, 24, 24, 
            19, 21, 20, 21, 
            34, 33, 34, 34},
        // block 4
            { 5, 6, 7, 8, 
            40, 25, 24, 40, 
            19, 21, 20, 146, 
            34, 33, 34, 34},
        // block 5
            { 1, 2, 18, 21, 
            31, 32, 34, 34, 
            21, 21, 20, 21, 
            33, 34, 34, 34},
        // block 6
        { 8, 1, 2, 18, 
            22, 31, 32, 34, 
            19, 21, 20, 21, 
            34, 33, 34, 34},
        // block 7
        { 1, 1, 2, 128, 
            22, 23, 22, 22, 
            1, 1, 2, 128, 
            22, 23, 22, 22},
        // block 8
        { 5, 6, 7, 8, 
            24, 25, 24, 24, 
            14, 15, 11, 12, 
            35, 36, 26, 27},
        // block 9
        { 1, 1, 2, 21, 
            22, 23, 22, 34, 
            1, 1, 18, 21, 
            31, 32, 34, 34},
        // block 10
        { 21, 21, 20, 21, 
            34, 33, 34, 34, 
            14, 15, 11, 12, 
            35, 36, 26, 27},
        // block 11
        { 21, 21, 20, 21, 
            37, 33, 34, 34, 
            9, 15, 11, 12, 
            35, 36, 26, 27},
        // block 12
        { 21, 21, 20, 21, 
            37, 33, 34, 161, 
            9, 10, 3, 4, 
            35, 36, 26, 27},
        // block 13
        { 21, 21, 20, 21, 
            34, 33, 34, 34, 
            16, 17, 20, 21, 
            28, 29, 30, 34},
        // block 14
        { 21, 21, 20, 21, 
            37, 33, 34, 34, 
            1, 16, 17, 21, 
            38, 39, 22, 30},
        // block 15
        { 1, 16, 17, 21, 
            28, 29, 22, 30, 
            44, 45, 3, 4, 
            89, 89, 42, 43},
        // block 16
        { 44, 45, 3, 4, 
            89, 89, 42, 43, 
            93, 94, 89, 89, 
            0, 0, 93, 94},
        // block 17
        { 1, 1, 2, 21, 
            28, 29, 22, 41, 
            44, 45, 3, 4, 
            89, 89, 42, 43},
        // block 18
        { 19, 21, 20, 21, 
            34, 33, 34, 34, 
            21, 21, 20, 21, 
            34, 33, 34, 34},
        // block 19
        { 21, 21, 20, 21, 
            34, 33, 34, 34, 
            21, 21, 20, 21, 
            37, 33, 34, 34},
        // block 20
        { 21, 21, 20, 21, 
            34, 33, 34, 34, 
            21, 21, 20, 21, 
            34, 33, 34, 34},
        // block 21
        { 1, 1, 2, 18, 
            22, 23, 22, 34, 
            1, 1, 2, 21, 
            22, 23, 22, 34},
        // block 22
        { 50, 51, 178, 177, 
            48, 49, 176, 175, 
            50, 51, 178, 177, 
            52, 53, 87, 88},
        // block 23
        { 46, 47, 174, 173,
            48, 49, 176, 175,
            50, 51, 178, 177,
            48, 49, 176, 175},
        // block 24
        { 69, 70, 63, 64, 
            65, 66, 71, 72, 
            73, 74, 75, 76, 
            77, 78, 97, 98},
        // block 25
        {  0,  0, 54, 80,
            57, 58, 59, 60,
            61, 62, 63, 64,
            65, 66, 67, 68},
        // block 26
        {   0,  0, 54, 56,
            57, 58, 59, 60,
            61, 62, 63, 64,
            65, 66, 67, 68},
        // block 27
        { 50, 51, 51+127, 177,
            48, 49, 49+127, 175,
            50, 51, 51+127, 177,
            48, 49, 49+127, 175},
        // block 28
        { 0,  0,  0,  0,
            0,  0,  0,  0,
           46, 47, 47+127, 46+127,
           48, 49, 49+127, 48+127},
        // block 29
        { 61, 62, 63, 64,
            65, 66, 67, 68,
            69, 70, 63, 64,
            65, 66, 71, 72},
        // block 30
        {  0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0, 54, 80,
           57, 58, 59, 60,},
        // block 31
        {  0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0, 54, 56,
           57, 58, 59, 60},
        // block 32
        { 73, 74, 75, 76, 
            77, 78, 97, 98, 
            99, 100, 2, 18, 
            22, 31, 32, 34},
        // block 33
        { 73, 74, 75, 76, 
            77, 78, 97, 98, 
            99, 100, 2, 18, 
            22, 23, 22, 34},
        // block 34
        { 0,  0, 79, 80,
            0,  0, 81, 82,
            0,  0, 83, 84,
            0,  0, 81, 82},
        // block 35
        { 0,  0, 81, 82,
            0,  0, 83, 84,
            0,  0, 81, 82,
            0,  0, 83, 84},
        // block 36
        { 0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0, 79, 80,
            0,  0, 83, 84 },
        // block 37
        { 0,  0, 81, 82,
            0,  0, 85, 86,
            0,  0,  0,  0,
            0,  0,  0,  0 },
        // block 38
        { 209, 208, 0, 0, 
            211, 210, 0, 0, 
            46, 47, 174, 173, 
            48, 49, 176, 175 },
        // block 39
        {209, 208, 81, 82, 
            211, 210, 83, 84, 
            46, 47, 174, 173, 
            48, 49, 176, 175},
        // block 40
        { 90, 90, 90, 90,
            90, 90, 90, 90,
             0,  0,  0,  0,
             0,  0,  0,  0},
        // block 41
        { 93, 94, 90, 90,
            0,  0, 93, 94,
            0,  0,  0,  0,
            0,  0,  0,  0},
        // block 42
        { 90, 90, 91, 82,
            90, 90, 92, 84,
             0,  0, 81, 82,
             0,  0, 83, 84},
        // block 43
        { 82+127, 81+127, 81, 82,
            84+127, 83+127, 83, 84,
            82+127, 81+127, 81, 82,
            84+127, 83+127, 83, 84},
        // block 44
        {21, 21, 20, 21, 
           34, 33, 34, 34,
           46, 47, 47+127, 46+127,
           48, 49, 49+127, 48+127},
        // block 45
        {82+127, 81+127, 81, 82,
            84+127, 83+127, 83, 84,
            0, 0, 0, 0,
            0, 0, 0, 0 },
    };
}

#endif