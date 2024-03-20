#ifndef BN_SPRITE_ITEMS_CHARACTER_H
#define BN_SPRITE_ITEMS_CHARACTER_H

#include "bn_sprite_item.h"

//{{BLOCK(character_bn_gfx)

//======================================================================
//
//	character_bn_gfx, 16x96@4, 
//	+ palette 16 entries, not compressed
//	+ 24 tiles not compressed
//	Total size: 32 + 768 = 800
//
//	Time-stamp: 2024-03-19, 20:23:40
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CHARACTER_BN_GFX_H
#define GRIT_CHARACTER_BN_GFX_H

#define character_bn_gfxTilesLen 768
extern const bn::tile character_bn_gfxTiles[24];

#define character_bn_gfxPalLen 32
extern const bn::color character_bn_gfxPal[16];

#endif // GRIT_CHARACTER_BN_GFX_H

//}}BLOCK(character_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item character(sprite_shape_size(sprite_shape::TALL, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(character_bn_gfxTiles, 24), bpp_mode::BPP_4, compression_type::NONE, 3), 
            sprite_palette_item(span<const color>(character_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

