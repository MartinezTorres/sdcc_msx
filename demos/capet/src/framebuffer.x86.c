#include <msxhal.h>

#ifdef LINUX

void fillFrameBuffer(uint8_t tiles[24][128],  uint8_t PNaddressH, uint16_t x, uint16_t y) {
	
	T_PN PN;

	(void)y;
	//uint16_t displayMapPosY = y;
	uint16_t displayMapPosX = x;

	uint8_t i,j;
	uint8_t x2=(displayMapPosX+0x20)>>6;
	uint8_t pv = ((x2&3)<<6);
	for (i=0; i<20; i++) {
		uint8_t *p = &tiles[19-i][(x2>>2)];
		uint8_t old = *p++;
		for (j=0; j<TILE_WIDTH; j++) {
			old = pv + (0x3F&(old<<3)) + *p++;
			PN[i][j]= old;
		}
	}	
	TMS9918_memcpy(PNaddressH<<8,&PN[0][0],sizeof(PN));		
}

#endif
