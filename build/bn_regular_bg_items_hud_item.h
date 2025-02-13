#ifndef BN_REGULAR_BG_ITEMS_HUD_ITEM_H
#define BN_REGULAR_BG_ITEMS_HUD_ITEM_H

#include "bn_regular_bg_item.h"

//{{BLOCK(hud_item_bn_gfx)

//======================================================================
//
//	hud_item_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 16 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 512 + 2048 = 2592
//
//	Time-stamp: 2025-02-12, 16:10:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_HUD_ITEM_BN_GFX_H
#define GRIT_HUD_ITEM_BN_GFX_H

#define hud_item_bn_gfxTilesLen 512
extern const bn::tile hud_item_bn_gfxTiles[16];

#define hud_item_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell hud_item_bn_gfxMap[1024];

#define hud_item_bn_gfxPalLen 32
extern const bn::color hud_item_bn_gfxPal[16];

#endif // GRIT_HUD_ITEM_BN_GFX_H

//}}BLOCK(hud_item_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item hud_item(
            regular_bg_tiles_item(span<const tile>(hud_item_bn_gfxTiles, 16), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(hud_item_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(hud_item_bn_gfxMap[0], size(32, 32), compression_type::NONE, 1, false));
}

#endif

