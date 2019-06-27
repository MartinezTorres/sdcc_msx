
#include <monospace.h>
#include <tms99X8.h>

////////////////////////////////////////////////////////////////////////
// Monospace Font API
//
// Text API where each character belongs to a Tile.
//

void M2_MS_initFontTiles(T_M2_MS_Font font) {

	memset(font,0,sizeof(T_M2_MS_Font));
}

void M2_MS_freeFontTiles(T_M2_MS_Font font) {

	uint8_t i,j,k;
	for (i=0; i<2; i++)
		for (j=0; j<3; j++)
			for (k=0; k<96; k++)
				Tiles_free(j,font[i][j][k]);

	memset(font,0,sizeof(T_M2_MS_Font));
}

void M2_MS_transformNull(const U8x8 source, U8x8 target) {
	uint8_t i;
	for (i=0; i<8; i++)
		target[i]=source[i];
}

void M2_MS_transformBorder(const U8x8 source, U8x8 target) {
	uint8_t i;
	for (i=0; i<8; i++)
		target[i]=source[i];

	for (i=0; i<7; i++)
		target[i] |= source[i+1];

	for (i=1; i<8; i++)
		target[i] |= source[i-1];

	for (i=0; i<8; i++)
		target[i] = target[i] | (target[i]<<1) | (target[i]>>1);
}

void M2_MS_setCharSimple(
	T_M2_MS_Font font, // Font to be setup
	EM2_RowPageFlags pages, // Pages for which this font is active
	uint8_t idx, // ASCII index corresponding to this tile
	const U8x8 shape, // Shape of the tile
	const U8x8 color // Color of the tile
) {
	TileIdx tileIdx;
	uint8_t page;
	uint16_t tileAddress;
	if (idx>127) idx=127;
	if (idx<32) idx=32;
	idx -=32;
	for (page=0; page<3; page++) {
		if ( (pages & (1<<page))==0 ) continue;
		
		tileIdx = Tiles_findAndAllocate(page);
		tileAddress = (((uint16_t)page)<<11) + (((uint16_t)tileIdx)<<3); 
		TMS99X8_memcpy(MODE2_ADDRESS_PG + tileAddress, shape, 8);
		TMS99X8_memcpy(MODE2_ADDRESS_CT + tileAddress, color, 8);

		font[0][page][idx]=tileIdx;
		font[1][page][idx]=tileIdx;
	}
}

void M2_MS_setCharDouble(
	T_M2_MS_Font font, // Font to be setup
	EM2_RowPageFlags pgpage, // Pages for which this font is active
	uint8_t idx, // ASCII index corresponding to this tile
	const U8x8 shape0, // Shape of the tile on buffer0
	const U8x8 color0, // Color of the tile on buffer0
	const U8x8 shape1, // Shape of the tile on buffer1
	const U8x8 color1 // Color of the tile on buffer1
) {
	TileIdx tileIdx0,tileIdx1;
	uint8_t page;
	uint16_t tileAddress;
	if (idx>127) idx=127;
	if (idx<32) idx=32;
	idx -=32;
	for (page=0; page<3; page++) {
		if ( (pgpage & (1<<page))==0 ) continue;
		
		tileIdx0 = Tiles_findAndAllocate(page);
		tileIdx1 = Tiles_findAndAllocate(page);
		
		tileAddress = (((uint16_t)page)<<11) + (((uint16_t)tileIdx0)<<3); 
		TMS99X8_memcpy(MODE2_ADDRESS_PG + tileAddress, shape0, 8);
		TMS99X8_memcpy(MODE2_ADDRESS_CT + tileAddress, color0, 8);
		
		printf("tileAddress %d %d\n",tileIdx0, MODE2_ADDRESS_PG + tileAddress);
		fflush(stdout);
		

		tileAddress = (((uint16_t)page)<<11) + (((uint16_t)tileIdx1)<<3); 
		TMS99X8_memcpy(MODE2_ADDRESS_PG + tileAddress, shape1, 8);
		TMS99X8_memcpy(MODE2_ADDRESS_CT + tileAddress, color1, 8);

		font[0][page][idx]=tileIdx0;
		font[1][page][idx]=tileIdx1;
	}
}

static uint8_t posX, posY;
void M2_MS_setPos  (const uint8_t x, const uint8_t y) { posX=x; posY=y; }
void M2_MS_getPos  (uint8_t *x, uint8_t *y) { *x=posX; *y=posY; }

void M2_MS_putchar (T_M2_MS_Font font, uint8_t idx) {
	
	if (idx>128) idx=32;
	if (idx<32) idx=32;
	idx -=32;
	
	if (posX>31) return;
	if (posY>23) return;
	
	{
		uint8_t  page = posY>>3; 
		uint16_t address = ((uint16_t)(posY)<<5)+posX;		
//		; 
		TMS99X8_memset(MODE2_ADDRESS_PN0 + address, font[0][page][idx] ,1);
		TMS99X8_memset(MODE2_ADDRESS_PN1 + address, font[1][page][idx] ,1);
		
		//printf("Print %c, (%d,%d) idx:%02X,%02X %04X\n",idx+32, posX, posY, font[0][page][idx], font[1][page][idx], MODE2_ADDRESS_PN0 + address);
		fflush(stdout);
	}
	
	posX++;
	if (posX==32) { posX=0; posY++; };
	if (posY==24) { posY=0; };
}


void M2_MS_puts(T_M2_MS_Font font, const char *msg) {
	
	while (*msg) 
		M2_MS_putchar(font, *msg++);
}

void M2_MS_printAt (T_M2_MS_Font font, uint8_t x, uint8_t y, const char *msg) {
	
	M2_MS_setPos(x,y);
	M2_MS_puts(font,msg);
}
