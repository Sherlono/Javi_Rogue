#ifndef BN_SPRITE_ITEMS_BIG_FLOOR_H
#define BN_SPRITE_ITEMS_BIG_FLOOR_H

#include "bn_sprite_item.h"

//{{BLOCK(big_floor_bn_gfx)

//======================================================================
//
//	big_floor_bn_gfx, 32x576@4, 
//	+ palette 16 entries, not compressed
//	+ 288 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 9216 = 9248
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BIG_FLOOR_BN_GFX_H
#define GRIT_BIG_FLOOR_BN_GFX_H

#define big_floor_bn_gfxTilesLen 9216
extern const bn::tile big_floor_bn_gfxTiles[288];

#define big_floor_bn_gfxPalLen 32
extern const bn::color big_floor_bn_gfxPal[16];

#endif // GRIT_BIG_FLOOR_BN_GFX_H

//}}BLOCK(big_floor_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item big_floor(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(big_floor_bn_gfxTiles, 288), bpp_mode::BPP_4, compression_type::NONE, 18), 
            sprite_palette_item(span<const color>(big_floor_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

