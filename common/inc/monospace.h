#pragma once
#include <tile.h>

////////////////////////////////////////////////////////////////////////
// Font API
//
//typedef U8x8 TFont[128];
//typedef U8x8 TFontD[2][128];

////////////////////////////////////////////////////////////////////////
// Monospace Font API
//
// Text API where each character belongs to a Tile.
//

typedef enum {
M2_MS_PAGES_NONE = 0,
M2_MS_PAGES_0    = 1,
M2_MS_PAGES_1    = 2,
M2_MS_PAGES_01   = 3,
M2_MS_PAGES_2    = 4,
M2_MS_PAGES_02   = 5,
M2_MS_PAGES_12   = 6,
M2_MS_PAGES_123  = 7,
}
EM2_MS_Pages;

typedef uint8_t T_M2_MS_Font[2][3][96]; // It's pretty big!
void M2_MS_freeFontTiles( T_M2_MS_Font font );

void M2_MS_transformNull(const U8x8 source, U8x8 target);
void M2_MS_transformBorder(const U8x8 source, U8x8 target);

void M2_MS_setCharSimple(
	T_M2_MS_Font font, // Font to be setup
	EM2_MS_Pages pgpage, // Pages for which this font is active
	uint8_t idx, // ASCII index corresponding to this tile
	const U8x8 shape, // Shape of the tile
	const U8x8 color // Color of the tile
);

void M2_MS_setCharDouble(
	T_M2_MS_Font font, // Font to be setup
	EM2_MS_Pages pgpage, // Pages for which this font is active
	uint8_t idx, // ASCII index corresponding to this tile
	const U8x8 shape0, // Shape of the tile on buffer0
	const U8x8 color0, // Color of the tile on buffer0
	const U8x8 shape1, // Shape of the tile on buffer1
	const U8x8 color1 // Color of the tile on buffer1
);

INLINE void M2_MS_setFontSimple( // It setups charachers between 32 and 127 (inclusive)
	T_M2_MS_Font font, // Font to be setup
	EM2_MS_Pages pages, // Pages for which this font is active
	const U8x8 shapes[96], // Font shapes 
	uint8_t background,	uint8_t foreground // Colors to use for the entire font
) {
	
	U8x8 color;
	uint8_t i;
	M2_MS_freeFontTiles(font);
	for (i=0; i<8; i++) color[i] = background + foreground; 
	for (i=0; i<96; i++) M2_MS_setCharSimple(font, pages, i+32, shapes[i], color);
}

INLINE void M2_MS_setFontDouble( // It setups charachers between 32 and 127 (inclusive)
	T_M2_MS_Font font, // Font to be setup
	EM2_MS_Pages pages, // Pages for which this font is active
	const U8x8 shapes[96], // Font shapes 
	void(*transform)(const U8x8, U8x8),
	uint8_t background0, uint8_t foreground0,
	uint8_t background1, uint8_t foreground1 // Colors to use for the entire font
) {
	uint8_t i;
	U8x8 color0, color1;
	U8x8 bold;
	M2_MS_freeFontTiles(font);
	if (transform==nullptr) transform = &M2_MS_transformNull;
	for (i=0; i<8; i++) color0[i] = background0 + foreground0; 
	for (i=0; i<8; i++) color1[i] = background1 + foreground1;
	for (i=0; i<96; i++) {
		(*transform)(shapes[i], bold);
		M2_MS_setCharDouble(
			font, 
			pages, 
			i+32, 
			shapes[i],
			color0,
			bold,
			color1);
	}
}

void M2_MS_setPos  (uint8_t x, uint8_t y);
void M2_MS_getPos  (uint8_t *x, uint8_t *y);

void M2_MS_putchar (T_M2_MS_Font font, uint8_t idx);
void M2_MS_puts    (T_M2_MS_Font font, const char *msg);
void M2_MS_printAt (T_M2_MS_Font font, uint8_t x, uint8_t y, const char *msg);
