#pragma once
#include <msxhal.h>
#include <tms9918.h>

////////////////////////////////////////////////////////////////////////
// Font API
//
typedef uint8_t TFont[128][8];

////////////////////////////////////////////////////////////////////////
// Tile Manager API
//
// Used to reserve and free tiles.
//
typedef uint8_t TileIdx;

TileIdx M2_findFreeTile(EM2_PGpage pgpage);
void    M2_allocTile   (EM2_PGpage pgpage, TileIdx tileIdx);
void    M2_freeTile    (EM2_PGpage pgpage, TileIdx tileIdx);
void    M2_freeAllTiles(EM2_PGpage pgpage);

////////////////////////////////////////////////////////////////////////
// Monospace Font API
//
// Text API where each character belongs to a Tile.
//

void M2_MS_loadFontTiles(EM2_PGpage pgpage, const TFont font, const U8x8 color, char min, char max);
void M2_MS_freeFontTiles(EM2_PGpage pgpage, char min, char max);
void M2_MS_print        (EM2_Buffer buffer, uint8_t x, uint8_t y, const char *msg);

////////////////////////////////////////////////////////////////////////
// Proportional Font API
//
// Text API where Tiles are generated for each message.
//

bool MODE2_PS_renderMsg(
	EM2_PGpage m2pgpage,
	TileIdx *slim, 
	TileIdx *bold, 
	const char *msg, 
	const TFont font, 
	const U8x8 colorSlim,
	const U8x8 colorBold,
	uint8_t padding);

void M2_PS_freeMsg(TileIdx *msg);
void M2_PS_print(EM2_Buffer buffer, uint8_t x, uint8_t y, TileIdx *msg);
