#ifndef BN_SPRITE_ITEMS_BIG_WALL_H
#define BN_SPRITE_ITEMS_BIG_WALL_H

#include "bn_sprite_item.h"

//{{BLOCK(big_wall_bn_gfx)

//======================================================================
//
//	big_wall_bn_gfx, 32x160@4, 
//	+ palette 16 entries, not compressed
//	+ 80 tiles not compressed
//	Total size: 32 + 2560 = 2592
//
//	Time-stamp: 2024-03-24, 13:37:36
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BIG_WALL_BN_GFX_H
#define GRIT_BIG_WALL_BN_GFX_H

#define big_wall_bn_gfxTilesLen 2560
extern const bn::tile big_wall_bn_gfxTiles[80];

#define big_wall_bn_gfxPalLen 32
extern const bn::color big_wall_bn_gfxPal[16];

#endif // GRIT_BIG_WALL_BN_GFX_H

//}}BLOCK(big_wall_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item big_wall(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(big_wall_bn_gfxTiles, 80), bpp_mode::BPP_4, compression_type::NONE, 5), 
            sprite_palette_item(span<const color>(big_wall_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

