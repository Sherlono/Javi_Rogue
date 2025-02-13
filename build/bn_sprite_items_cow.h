#ifndef BN_SPRITE_ITEMS_COW_H
#define BN_SPRITE_ITEMS_COW_H

#include "bn_sprite_item.h"

//{{BLOCK(cow_bn_gfx)

//======================================================================
//
//	cow_bn_gfx, 32x32@4, 
//	+ palette 16 entries, not compressed
//	+ 16 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 512 = 544
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_COW_BN_GFX_H
#define GRIT_COW_BN_GFX_H

#define cow_bn_gfxTilesLen 512
extern const bn::tile cow_bn_gfxTiles[16];

#define cow_bn_gfxPalLen 32
extern const bn::color cow_bn_gfxPal[16];

#endif // GRIT_COW_BN_GFX_H

//}}BLOCK(cow_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item cow(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(cow_bn_gfxTiles, 16), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(cow_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

