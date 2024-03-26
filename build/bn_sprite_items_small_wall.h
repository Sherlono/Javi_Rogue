#ifndef BN_SPRITE_ITEMS_SMALL_WALL_H
#define BN_SPRITE_ITEMS_SMALL_WALL_H

#include "bn_sprite_item.h"

//{{BLOCK(small_wall_bn_gfx)

//======================================================================
//
//	small_wall_bn_gfx, 16x256@4, 
//	+ palette 16 entries, not compressed
//	+ 64 tiles not compressed
//	Total size: 32 + 2048 = 2080
//
//	Time-stamp: 2024-03-25, 15:29:12
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SMALL_WALL_BN_GFX_H
#define GRIT_SMALL_WALL_BN_GFX_H

#define small_wall_bn_gfxTilesLen 2048
extern const bn::tile small_wall_bn_gfxTiles[64];

#define small_wall_bn_gfxPalLen 32
extern const bn::color small_wall_bn_gfxPal[16];

#endif // GRIT_SMALL_WALL_BN_GFX_H

//}}BLOCK(small_wall_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item small_wall(sprite_shape_size(sprite_shape::TALL, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(small_wall_bn_gfxTiles, 64), bpp_mode::BPP_4, compression_type::NONE, 8), 
            sprite_palette_item(span<const color>(small_wall_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

