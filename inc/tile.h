#pragma once
#include <msxhal.h>
#include <tms99X8.h>

////////////////////////////////////////////////////////////////////////
// Tile Manager API
//
// Used to reserve and free tiles.
//
typedef uint8_t TileIdx;

void    Tiles_init();
TileIdx Tiles_findAndAllocate(RowPageIdx rowPageIdx);
void    Tiles_alloc   (RowPageIdx rowPageIdx, TileIdx tileIdx);
void    Tiles_free    (RowPageIdx rowPageIdx, TileIdx tileIdx);
INLINE void  Tiles_freeAll() { Tiles_init(); }

/*
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
*/
