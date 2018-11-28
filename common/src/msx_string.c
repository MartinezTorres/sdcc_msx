#include <string.h>

/*

// 16 bit pattern manipulation functions




// Tile and ASCII rutines
static uint8_t ascii2tiles[256]; // Tiles 0x10 to 0x1F reserved for the bloodmeter.

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
