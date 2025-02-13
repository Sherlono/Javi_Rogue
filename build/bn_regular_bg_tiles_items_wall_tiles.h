#ifndef BN_REGULAR_BG_TILES_ITEMS_WALL_TILES_H
#define BN_REGULAR_BG_TILES_ITEMS_WALL_TILES_H

#include "bn_regular_bg_tiles_item.h"

//{{BLOCK(wall_tiles_bn_gfx)

//======================================================================
//
//	wall_tiles_bn_gfx, 432x8@4, 
//	+ 54 tiles not compressed
//	Total size: 1728 = 1728
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_WALL_TILES_BN_GFX_H
#define GRIT_WALL_TILES_BN_GFX_H

#define wall_tiles_bn_gfxTilesLen 1728
extern const bn::tile wall_tiles_bn_gfxTiles[54];

#endif // GRIT_WALL_TILES_BN_GFX_H

//}}BLOCK(wall_tiles_bn_gfx)

namespace bn::regular_bg_tiles_items
{
    constexpr inline regular_bg_tiles_item wall_tiles(
            span<const tile>(wall_tiles_bn_gfxTiles, 54), bpp_mode::BPP_4, compression_type::NONE);
}

#endif

