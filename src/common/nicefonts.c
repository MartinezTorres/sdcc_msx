#include <nicefonts.h>

#define INIT(a,v) { register uint8_t value = v; register uint8_t sz = sizeof(a); register uint8_t *p = (uint8_t *)a; while (sz--) *p++=value; }

static void pasteCharacter( uint32_t workAreaSlim[8], uint32_t workAreaBold[8], const TFont font, uint8_t c, uint8_t maxDisplacement) {

	uint8_t i;
	for (i=0; i<8; i++) workAreaSlim[i] |= ((uint32_t)font[c][i]) << maxDisplacement;

	for (i=0; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i]) << (maxDisplacement+1);
	for (i=0; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i]) << (maxDisplacement+0);
	for (i=0; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i]) << (maxDisplacement-1);

	for (i=1; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i-1]) << (maxDisplacement+1);
	for (i=1; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i-1]) << (maxDisplacement+0);
	for (i=1; i<8; i++) workAreaBold[i] |= ((uint32_t)font[c][i-1]) << (maxDisplacement-1);

	for (i=0; i<7; i++) workAreaBold[i] |= ((uint32_t)font[c][i+1]) << (maxDisplacement+1);
	for (i=0; i<7; i++) workAreaBold[i] |= ((uint32_t)font[c][i+1]) << (maxDisplacement+0);
	for (i=0; i<7; i++) workAreaBold[i] |= ((uint32_t)font[c][i+1]) << (maxDisplacement-1);

}

static uint8_t writeShape( uint32_t workArea[8], uint8_t freeTiles[256] ) {
	
	
	U8x8 shape;
	uint8_t freeTile = 1;
	{
		uint8_t i;
		for (i=0; i<8; i++) {
			shape[i] = workArea[i]>>24;
			workArea[i] <<= 8;
		}
	}
	while (freeTile && freeTiles[freeTile]==0) freeTile++;

	TMS9918_memcpy(ADDRESS_PG + (((uint16_t)freeTile)<<3), shape, 8);

	freeTiles[freeTile] = 0;
	return freeTile;
}

void initRenderedText(uint8_t *slim, uint8_t *bold, uint8_t freeTiles[256], const uint8_t *str, const TFont font, uint8_t padding) {
	
	uint8_t maxDisplacement = 32-8-padding;
	uint32_t workAreaSlim[8];
	uint32_t workAreaBold[8];
	INIT(workAreaSlim,0);
	INIT(workAreaBold,0);

	if (*str==0) return;
	{
		uint8_t c = *str++;
		pasteCharacter(workAreaSlim,workAreaBold,font,c,maxDisplacement);
		maxDisplacement -= 8;
	}
	while (*str) {
		uint8_t c = *str++;
		
		while (maxDisplacement < 32-8-padding + 2) {
			uint8_t i;
			for (i=0; i<8; i++) 
				if ((workAreaSlim[i] & (((uint32_t)font[c][i]) << maxDisplacement)) != 0) 
					goto out;
			maxDisplacement++;
		}
		out:
		maxDisplacement -= 2;
		pasteCharacter(workAreaSlim,workAreaBold,font,c,maxDisplacement);
		
		maxDisplacement -= 8;
		
		if (maxDisplacement<10) {
			
			*slim++ = writeShape(workAreaSlim, freeTiles);
			*bold++ = writeShape(workAreaBold, freeTiles);
			maxDisplacement += 8;
		}
	}
	while (maxDisplacement<(32-8+padding)) {

		*slim++ = writeShape(workAreaSlim, freeTiles);
		*bold++ = writeShape(workAreaBold, freeTiles);
		maxDisplacement += 8;
	}
	
	*slim++ = 0;
	*bold++ = 0;
}
