#ifndef BN_SPRITE_ITEMS_HUGE_WALL_H
#define BN_SPRITE_ITEMS_HUGE_WALL_H

#include "bn_sprite_item.h"

//{{BLOCK(huge_wall_bn_gfx)

//======================================================================
//
//	huge_wall_bn_gfx, 32x192@4, 
//	+ palette 16 entries, not compressed
//	+ 96 tiles Metatiled by 4x8 not compressed
//	Total size: 32 + 3072 = 3104
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_HUGE_WALL_BN_GFX_H
#define GRIT_HUGE_WALL_BN_GFX_H

#define huge_wall_bn_gfxTilesLen 3072
extern const bn::tile huge_wall_bn_gfxTiles[96];

#define huge_wall_bn_gfxPalLen 32
extern const bn::color huge_wall_bn_gfxPal[16];

#endif // GRIT_HUGE_WALL_BN_GFX_H

//}}BLOCK(huge_wall_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item huge_wall(sprite_shape_size(sprite_shape::TALL, sprite_size::HUGE), 
            sprite_tiles_item(span<const tile>(huge_wall_bn_gfxTiles, 96), bpp_mode::BPP_4, compression_type::NONE, 3), 
            sprite_palette_item(span<const color>(huge_wall_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

