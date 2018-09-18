#include <nicefonts.h>

#define INIT(a,v) { register uint8_t value = v; register uint8_t sz = sizeof(a); register uint8_t *p = (uint8_t *)a; while (sz--) *p++=value; }

#define uintXX_t uint32_t
#define XX (8*sizeof(uintXX_t))

static void pasteCharacter( uintXX_t workAreaSlim[8], uintXX_t workAreaBold[8], const TFont font, uint8_t c, uint8_t maxDisplacement_) {

	uint8_t i;
	register uint8_t maxDisplacement = maxDisplacement_;
	register uint8_t *f = font[c];
	for (i=0; i<8; i++) {
		
		uintXX_t fi = f[i];
		
		workAreaSlim[i] |= fi << maxDisplacement;
		
		workAreaBold[i] |= 
			(fi << (maxDisplacement+1)) | 
			(fi << (maxDisplacement+0)) | 
			(fi << (maxDisplacement-1));
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

static uint8_t writeShape( uintXX_t workArea[8], uint8_t freeTiles[256] ) {
	
	
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
	
	uint8_t maxDisplacement = XX-8-padding;
	uintXX_t workAreaSlim[8];
	uintXX_t workAreaBold[8];
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
		uint8_t mx = min(XX-8-padding + 2, maxDisplacement+6);
		
		while (maxDisplacement < mx) {
			uint8_t i;
			for (i=0; i<8; i++) 
				if ((workAreaSlim[i] & (((uintXX_t)font[c][i]) << maxDisplacement)) != 0) 
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
	while (maxDisplacement<(XX-8+padding)) {

		*slim++ = writeShape(workAreaSlim, freeTiles);
		*bold++ = writeShape(workAreaBold, freeTiles);
		maxDisplacement += 8;
	}
	
	*slim++ = 0;
	*bold++ = 0;
}
