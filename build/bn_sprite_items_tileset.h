#ifndef BN_SPRITE_ITEMS_TILESET_H
#define BN_SPRITE_ITEMS_TILESET_H

#include "bn_sprite_item.h"

//{{BLOCK(tileset_bn_gfx)

//======================================================================
//
//	tileset_bn_gfx, 16x704@4, 
//	+ palette 16 entries, not compressed
//	+ 176 tiles not compressed
//	Total size: 32 + 5632 = 5664
//
//	Time-stamp: 2024-03-19, 20:23:40
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TILESET_BN_GFX_H
#define GRIT_TILESET_BN_GFX_H

#define tileset_bn_gfxTilesLen 5632
extern const bn::tile tileset_bn_gfxTiles[176];

#define tileset_bn_gfxPalLen 32
extern const bn::color tileset_bn_gfxPal[16];

#endif // GRIT_TILESET_BN_GFX_H

//}}BLOCK(tileset_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item tileset(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(tileset_bn_gfxTiles, 176), bpp_mode::BPP_4, compression_type::NONE, 44), 
            sprite_palette_item(span<const color>(tileset_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

