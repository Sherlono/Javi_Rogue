#ifndef BN_SPRITE_ITEMS_SMALL_FLOOR_H
#define BN_SPRITE_ITEMS_SMALL_FLOOR_H

#include "bn_sprite_item.h"

//{{BLOCK(small_floor_bn_gfx)

//======================================================================
//
//	small_floor_bn_gfx, 32x80@4, 
//	+ palette 16 entries, not compressed
//	+ 40 tiles not compressed
//	Total size: 32 + 1280 = 1312
//
//	Time-stamp: 2024-03-24, 13:37:36
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SMALL_FLOOR_BN_GFX_H
#define GRIT_SMALL_FLOOR_BN_GFX_H

#define small_floor_bn_gfxTilesLen 1280
extern const bn::tile small_floor_bn_gfxTiles[40];

#define small_floor_bn_gfxPalLen 32
extern const bn::color small_floor_bn_gfxPal[16];

#endif // GRIT_SMALL_FLOOR_BN_GFX_H

//}}BLOCK(small_floor_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item small_floor(sprite_shape_size(sprite_shape::WIDE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(small_floor_bn_gfxTiles, 40), bpp_mode::BPP_4, compression_type::NONE, 5), 
            sprite_palette_item(span<const color>(small_floor_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

