#ifndef BN_BG_PALETTE_ITEMS_FLOOR_PALETTE_H
#define BN_BG_PALETTE_ITEMS_FLOOR_PALETTE_H

#include "bn_bg_palette_item.h"

//{{BLOCK(floor_palette_bn_gfx)

//======================================================================
//
//	floor_palette_bn_gfx, 8x8@4, 
//	+ palette 16 entries, not compressed
//	Total size: 32 = 32
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FLOOR_PALETTE_BN_GFX_H
#define GRIT_FLOOR_PALETTE_BN_GFX_H

#define floor_palette_bn_gfxPalLen 32
extern const bn::color floor_palette_bn_gfxPal[32];

#endif // GRIT_FLOOR_PALETTE_BN_GFX_H

//}}BLOCK(floor_palette_bn_gfx)

namespace bn::bg_palette_items
{
    constexpr inline bg_palette_item floor_palette(span<const color>(floor_palette_bn_gfxPal, 32), 
            bpp_mode::BPP_4, compression_type::NONE);
}

#endif

