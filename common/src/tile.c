#include <tile.h>

static uint8_t tileOccupancyMap[3][32];

////////////////////////////////////////////////////////////////////////
// Tile Manager API
//
// Used to reserve and free tiles.
//

TileIdx M2_findFreeTile(EM2_PGpage pgpage) {

	uint8_t i, val;
	uint8_t *p = &tileOccupancyMap[pgpage][0];
	
	for (i=0; (i<32) && (*p==0xFF); i++, p++);
	
	if (i==32) return 0;

	val = *p;
	i = i << 3;
	while (val & 0x01) {
		i++;
		val = val >> 1;
	}
	return i;
}

void    M2_allocTile   (EM2_PGpage pgpage, TileIdx tileIdx) {
	
	uint8_t *val = &tileOccupancyMap[pgpage][tileIdx>>3];
	*val = *val | (1<<(tileIdx&0x07));	
}

void    M2_freeTile    (EM2_PGpage pgpage, TileIdx tileIdx) {

	uint8_t *val = &tileOccupancyMap[pgpage][tileIdx>>3];
	*val = *val & ~(1<<(tileIdx&0x07));	
}

void    M2_freeAllTiles(EM2_PGpage pgpage) {
	
	uint8_t i=32;
	uint8_t *p = &tileOccupancyMap[pgpage][0];
	while (i--)
		*p++ = 0;
	
	M2_allocTile(pgpage,0); // Tile 0 is reserved for signaling the end of a message
}
/*


////////////////////////////////////////////////////////////////////////
// Monospace Font API
//
// Text API where each character belongs to a Tile.
//

//typedef struct {
//	uint8_t conv[256];
//}
//T_MS_Font;

void M2_MS_loadFont(
	EM2_PGpage pgpage, 
	const T_MS_Font *msFont, 
	const TFont font, 
	const U8x8 color, 
	uint8_t min, uint8_t max) {
	
	register uint8_t i = min;
	for (i=min, i<max; i++) {
		TileIdx freeTile = M2_findFreeTile(pgpage);
		TMS9918_memcpy(ADDRESS_PG + (((uint16_t)freeTile)<<3), font[i], 8);
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)freeTile)<<3), color, 8);
		msFont.idx[i] = freeTile;
	}
}

void M2_MS_freeFont(EM2_PGpage pgpage, const T_MS_Font *msFont, char min, char max) {
	
	register uint8_t i = min;
	for (i=min, i<max; i++) 
		M2_freeTile(pgpage, msFont.idx[i]);
}

void M2_MS_print(EM2_Buffer buffer, const T_MS_Font *msFont, const char *msg, uint8_t x, uint8_t y) {
	
	uint8_t tiles[32];
	register uint8_t l = 0;
	{
		register const char *src = msg;
		register uint8_t *dst = &tiles[0];
		while (TRUE) {
			register uint8_t v = *src++;
			if (v==0) break;
			l++;
			*dst++ = msFont.idx[v];
		}
	}
	TMS9918_memcpy(baseAddress + x+(y<<5),tiles,l);
}





// Precondition, freeTiles must have at least 1 spot!
uint8_t addASCIITile(uint8_t freeTiles[256], uint8_t ascii, const U8x8 shape, const U8x8 color) {
	
	uint8_t freeTile = 0;
	while (freeTiles[freeTile]==0) freeTile++;
	
	TMS9918_memcpy(ADDRESS_PG + (((uint16_t)freeTile)<<3), shape, 8);
	TMS9918_memcpy(ADDRESS_CT + (((uint16_t)freeTile)<<3), color, 8);

	freeTiles[freeTile] = 0;
	ascii2tiles[ascii] = freeTile;
	return freeTile;
}

// Text rutines
void uint16ToString(char *msg, uint16_t v, uint8_t enableZeroPad) {
	
	register char *p = msg;
	char c;
	c = 0; while (v>10000) { c++; v-=10000; } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>1000 ) { c++; v-=1000;  } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>100  ) { c++; v-=100;   } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>10   ) { c++; v-=10;    } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>1    ) { c++; v-=1;     } if (c) *p++ = '0'+c;
	*p++=0;
}










#define INIT(a,v) { register uint8_t value = v; register uint8_t sz = sizeof(a); register uint8_t *p = (uint8_t *)a; while (sz--) *p++=value; }

#define uintXX_t uint16_t
#define XX (8*sizeof(uintXX_t))

static void pasteCharacter(uintXX_t workAreaSlim[8], uintXX_t workAreaBold[8], const TFont font, uint8_t c, uint8_t maxDisplacement_) {

	uint8_t i;
	register uint8_t maxDisplacement = maxDisplacement_;
	register const uint8_t *f = font[c];
	for (i=0; i<8; i++) {
		
		workAreaSlim[i] |= ((uintXX_t)f[i]) << maxDisplacement;
		
		workAreaBold[i] |= 
			(((uintXX_t)f[i]) << (maxDisplacement+1)) | 
			(((uintXX_t)f[i]) << (maxDisplacement+0)) | 
			(((uintXX_t)f[i]) << (maxDisplacement-1));
	}

	for (i=0; i<7; i++) {
		
		workAreaBold[i+1] |= ((uintXX_t)f[i]) << (maxDisplacement+1);
		workAreaBold[i+1] |= ((uintXX_t)f[i]) << (maxDisplacement+0);
		workAreaBold[i+1] |= ((uintXX_t)f[i]) << (maxDisplacement-1);

		workAreaBold[i] |= ((uintXX_t)f[i+1]) << (maxDisplacement+1);
		workAreaBold[i] |= ((uintXX_t)f[i+1]) << (maxDisplacement+0);
		workAreaBold[i] |= ((uintXX_t)f[i+1]) << (maxDisplacement-1);
	}
}

static uint8_t writeShape(uintXX_t workArea[8], uint8_t freeTiles[256]) {
	
	
	U8x8 shape;
	uint8_t freeTile = 1;
	{
		uint8_t i,j;
		j = 0;
		for (i=0; i<8; i++) {
			shape[i] = workArea[i]>>(XX-8);
			j = j || shape[i];
			workArea[i] <<= 8;
		}
		if (!j) return 0;
	}
	while (freeTile && freeTiles[freeTile]==0) freeTile++;

	TMS9918_memcpy(ADDRESS_PG + (((uint16_t)freeTile)<<3), shape, 8);

	freeTiles[freeTile] = 0;
	return freeTile;
}

void initRenderedText(uint8_t *slim, uint8_t *bold, uint8_t freeTiles[256], const uint8_t *str, const TFont font, uint8_t padding) {
	
	int8_t maxDisplacement = XX-8-padding;
	uintXX_t workAreaSlim[8];
	uintXX_t workAreaBold[8];
	INIT(workAreaSlim,0);
	INIT(workAreaBold,0);

	if (*str==0) return;
	{
		uint8_t c = *str++;
		pasteCharacter(workAreaSlim,workAreaBold,font,c,maxDisplacement);
		maxDisplacement -= 8;
//		printf("A: %d\n",maxDisplacement);
	}
	while (*str) {
		uint8_t c = *str;
		int8_t mx = min((int8_t)(XX-8), maxDisplacement+6);
//		printf("MX A: %d %d\n",maxDisplacement, mx);
		
		while (maxDisplacement < mx) {
			uint8_t i;
			if (maxDisplacement>=0)
				for (i=0; i<8; i++) 
					if ((workAreaSlim[i] & (((uintXX_t)font[c][i]) << maxDisplacement)) != 0) 
						goto out;
			if (maxDisplacement< 0)
				for (i=0; i<8; i++) 
					if ((workAreaSlim[i] & (((uintXX_t)font[c][i]) >> -maxDisplacement)) != 0) 
						goto out;
			maxDisplacement++;
		}
		out:

//		printf("MX B: %d %d\n\n",maxDisplacement, mx);
		
		if (maxDisplacement<=3) {
			
			*slim++ = writeShape(workAreaSlim, freeTiles);
			*bold++ = writeShape(workAreaBold, freeTiles);
			maxDisplacement += 8;
			continue;
		}
		
		maxDisplacement -= 3;
		pasteCharacter(workAreaSlim,workAreaBold,font,c,maxDisplacement);
		maxDisplacement -= 8;
		str++;
	}
	while (TRUE) {
		
		uint8_t vs, vb;

		*slim++ = vs = writeShape(workAreaSlim, freeTiles);
		*bold++ = vb = writeShape(workAreaBold, freeTiles);
		if (!(vs || vb)) break;
		
	}
}


void TMS9918_printStr(uint16_t baseAddress, uint8_t x, uint8_t y, const char *msg) {

	uint8_t tiles[32];
	register uint8_t l = 0;
	{
		register const char *src = msg;
		register uint8_t *dst = &tiles[0];
		while (TRUE) {
			register uint8_t v = *src++;
			if (v==0) break;
			l++;
			*dst++ = ascii2tiles[v];
		}
	}
	TMS9918_memcpy(baseAddress + x+(y<<5),tiles,l);
}
*/
