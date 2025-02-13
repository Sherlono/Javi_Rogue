#ifndef BN_SPRITE_ITEMS_SMALL_WALL_H
#define BN_SPRITE_ITEMS_SMALL_WALL_H

#include "bn_sprite_item.h"

//{{BLOCK(small_wall_bn_gfx)

//======================================================================
//
//	small_wall_bn_gfx, 16x224@4, 
//	+ palette 16 entries, not compressed
//	+ 56 tiles Metatiled by 2x4 not compressed
//	Total size: 32 + 1792 = 1824
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SMALL_WALL_BN_GFX_H
#define GRIT_SMALL_WALL_BN_GFX_H

#define small_wall_bn_gfxTilesLen 1792
extern const bn::tile small_wall_bn_gfxTiles[56];

#define small_wall_bn_gfxPalLen 32
extern const bn::color small_wall_bn_gfxPal[16];

#endif // GRIT_SMALL_WALL_BN_GFX_H

//}}BLOCK(small_wall_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item small_wall(sprite_shape_size(sprite_shape::TALL, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(small_wall_bn_gfxTiles, 56), bpp_mode::BPP_4, compression_type::NONE, 7), 
            sprite_palette_item(span<const color>(small_wall_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

