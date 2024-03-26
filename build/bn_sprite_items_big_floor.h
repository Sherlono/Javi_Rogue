#ifndef BN_SPRITE_ITEMS_BIG_FLOOR_H
#define BN_SPRITE_ITEMS_BIG_FLOOR_H

#include "bn_sprite_item.h"

//{{BLOCK(big_floor_bn_gfx)

//======================================================================
//
//	big_floor_bn_gfx, 32x320@4, 
//	+ palette 16 entries, not compressed
//	+ 160 tiles not compressed
//	Total size: 32 + 5120 = 5152
//
//	Time-stamp: 2024-03-25, 15:29:12
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BIG_FLOOR_BN_GFX_H
#define GRIT_BIG_FLOOR_BN_GFX_H

#define big_floor_bn_gfxTilesLen 5120
extern const bn::tile big_floor_bn_gfxTiles[160];

#define big_floor_bn_gfxPalLen 32
extern const bn::color big_floor_bn_gfxPal[16];

#endif // GRIT_BIG_FLOOR_BN_GFX_H

//}}BLOCK(big_floor_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item big_floor(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(big_floor_bn_gfxTiles, 160), bpp_mode::BPP_4, compression_type::NONE, 10), 
            sprite_palette_item(span<const color>(big_floor_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

