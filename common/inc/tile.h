#pragma once
#include <msxhal.h>
#include <tms9918.h>

////////////////////////////////////////////////////////////////////////
// Font API
//
typedef U8x8 TFont[128];

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

typedef struct { uint8_t conv[256]; }
T_M2_MS_Status;

void M2_MS_clearpage();

void M2_MS_loadChar(EM2_PGpage pgpage, const uint8_t idx, const U8x8 shape, const U8x8 color);
void M2_MS_freeChar(EM2_PGpage pgpage, const uint8_t idx);

void M2_MS_setPos  (EM2_Buffer buffer, const uint8_t x, const uint8_t y);
void M2_MS_putchar (EM2_Buffer buffer, const uint8_t idx);
void M2_MS_puts    (EM2_Buffer buffer, const char *msg);

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
