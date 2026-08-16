#ifndef ACTORS_DATA_H
#define ACTORS_DATA_H

#include "bn_array.h"

#include "jv_actors_fwd.h"

namespace jv:: Actor_data{
    enum Id {Player, Bad_Cat, Pale_Tongue, Pale_Finger, Snakes, Cow, Fox};

    struct graphics_data{
        constexpr graphics_data(const uint8_t w_f, const bn::sprite_item& sprt_i_ref): wait_frames(w_f), sprite_item_ref(sprt_i_ref) {}

        const uint8_t wait_frames;
        const bn::sprite_item& sprite_item_ref;
    };

    constexpr bn::array<graphics_data, ACTOR_COUNT> data = {
        graphics_data( 4, bn::sprite_items::good_cat), 
        graphics_data( 4, bn::sprite_items::bad_cat), 
        graphics_data( 4, bn::sprite_items::pale_tongue), 
        graphics_data( 4, bn::sprite_items::pale_finger), 
        graphics_data(10, bn::sprite_items::snakes),
        graphics_data(10, bn::sprite_items::cow), 
        graphics_data(10, bn::sprite_items::fox)
    };

    struct meta_data{
        constexpr meta_data(const int8_t y_off, const uint8_t a_set,
                            const uint8_t v_range, const uint8_t a_range,
                            const uint8_t atk_cd,
                            const bn::size half_size, const bn::size rect_s): 
            sprt_y_offset(y_off), anim_set(a_set),
            view_range(v_range), attack_range(a_range),
            atk_end_cooldown(atk_cd),
            on_screen_offset(bn::size(half_size.width() + 120, half_size.height() + y_off + 80)),
            rect_shape(rect_s) {}

        const int8_t sprt_y_offset;
        const uint8_t anim_set, view_range, attack_range, atk_end_cooldown;
        const bn::size on_screen_offset, rect_shape;
    };

    constexpr bn::array<meta_data, ACTOR_COUNT> meta = {
        meta_data( 8, 0,46,18,40,{16, 16},{16, 16}),   // good cat
        meta_data( 8, 0,46,18,60,{16, 16},{16, 16}),   // bad cat
        meta_data( 8, 0,46,20,75,{16, 16},{16, 16}),   // pale tongue
        meta_data(24, 0,70,40,60,{32, 58},{16, 16}),  // pale finger
        meta_data(-2, 1,46,18,60,{16, 16},{12, 32}),   // snakes
        meta_data( 8, 1, 1, 1, 1,{16, 16},{20, 20}),   // cow
        meta_data( 8, 0, 1, 1, 1,{16, 16},{20, 20})    // fox
    };
    
}

namespace jv::animation{
    constexpr int MAX_FRAMES = 6;
    enum class Id {Walk, Attack, Die, Idle};
    
    constexpr bn::array<uint16_t, 4> W_down[2] =    {{0, 1, 0, 2}, { 0, 1, 2, 3}};
    constexpr bn::array<uint16_t, 4> W_horiz[2] =   {{3, 4, 3, 5}, { 4, 5, 6, 7}};
    constexpr bn::array<uint16_t, 4> W_up[2] =      {{6, 7, 6, 8}, { 8, 9,10,11}};
    constexpr bn::array<uint16_t, 4> Atk_do[2] =    {{0, 9,10,11}, {12,13,14,15}};
    constexpr bn::array<uint16_t, 4> Atk_horiz[2] = {{3,12,13,14}, {16,17,18,19}};
    constexpr bn::array<uint16_t, 4> Atk_up[2] =    {{6,15,16,17}, {20,21,22,23}};
    
    constexpr bn::array<uint16_t, 4> idle[2] = {{18, 19, 18, 20}, { 0, 1, 2, 3}};     // Idle down
    constexpr bn::array<uint16_t, 5> hurt[2] = {{21, 22, 21, 22, 22}, {24, 25, 24, 25, 24}}; // Hurt down
}

#endif