#ifndef BN_REGULAR_BG_TILES_ITEMS_FLOOR_TILES_H
#define BN_REGULAR_BG_TILES_ITEMS_FLOOR_TILES_H

#include "bn_regular_bg_tiles_item.h"

//{{BLOCK(floor_tiles_bn_gfx)

//======================================================================
//
//	floor_tiles_bn_gfx, 368x8@4, 
//	+ 46 tiles not compressed
//	Total size: 1472 = 1472
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FLOOR_TILES_BN_GFX_H
#define GRIT_FLOOR_TILES_BN_GFX_H

#define floor_tiles_bn_gfxTilesLen 1472
extern const bn::tile floor_tiles_bn_gfxTiles[46];

#endif // GRIT_FLOOR_TILES_BN_GFX_H

//}}BLOCK(floor_tiles_bn_gfx)

namespace bn::regular_bg_tiles_items
{
    constexpr inline regular_bg_tiles_item floor_tiles(
            span<const tile>(floor_tiles_bn_gfxTiles, 46), bpp_mode::BPP_4, compression_type::NONE);
}

#endif

