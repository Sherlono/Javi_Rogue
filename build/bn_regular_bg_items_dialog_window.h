#ifndef BN_REGULAR_BG_ITEMS_DIALOG_WINDOW_H
#define BN_REGULAR_BG_ITEMS_DIALOG_WINDOW_H

#include "bn_regular_bg_item.h"

//{{BLOCK(dialog_window_bn_gfx)

//======================================================================
//
//	dialog_window_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 7 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 224 + 2048 = 2304
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DIALOG_WINDOW_BN_GFX_H
#define GRIT_DIALOG_WINDOW_BN_GFX_H

#define dialog_window_bn_gfxTilesLen 224
extern const bn::tile dialog_window_bn_gfxTiles[7];

#define dialog_window_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell dialog_window_bn_gfxMap[1024];

#define dialog_window_bn_gfxPalLen 32
extern const bn::color dialog_window_bn_gfxPal[16];

#endif // GRIT_DIALOG_WINDOW_BN_GFX_H

//}}BLOCK(dialog_window_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item dialog_window(
            regular_bg_tiles_item(span<const tile>(dialog_window_bn_gfxTiles, 7), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(dialog_window_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(dialog_window_bn_gfxMap[0], size(32, 32), compression_type::NONE, 1, false));
}

#endif

