#include <capetsrace.h>

void initHeadSprites(const U16x16 hair[2], const U16x16 face[4]) {
	
	uint16_t tmp[16];
	uint8_t i, j, k;
	TMS9918_setFlags(TMS9918_M2 | TMS9918_GINT | TMS9918_MEM416K);
	
	writeSprite16(SPRITE_HAIR_MA,load16(tmp,hair[0]));
	writeSprite16(SPRITE_HAIR_LC,load16(tmp,hair[1]));
	for (k=0; k<2; k++) {
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				uint8_t jj=j;
				load16(tmp,face[i]);
				while (jj--) rotate16(tmp);
				if (k==0) {
					writeSprite16(SPRITE_HEAD_RIGHT+((j*4+i)<<2),tmp);
				} else {
					hflip16(tmp);
					writeSprite16(SPRITE_HEAD_LEFT+((0xF & (16-(j*4+i)))<<2),tmp);
				}
			}
		}
	}
	
	TMS9918_setFlags(TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K);	
}

void initMapTiles(const U8x8 shapes[8], const U8x8 colors[8]) {
	

	uint8_t h, cl, cr, l, mr, ml;
	
	for (h=0; h<4; h++) {
		uint8_t PG[64][8];
		for (cl=0; cl<8; cl++) {
			for (cr=0; cr<8; cr++) {
				uint8_t *p;
				const uint8_t *pcl;
				const uint8_t *pcr;
				p = PG[(cl<<3)+(cr<<0)];
				pcl = (const uint8_t *)&shapes[cl][0];
				pcr = (const uint8_t *)&shapes[cr][0];
				ml = 0xFF;
				mr = 0xFF;
				for (l=0; l<8; l++)
					*p++ = (((*pcl++) << (h*(cl?2:1)))&ml) | (( (*pcr++) >> (8-h*(cr?2:1)))&mr);
			}
		}
		TMS9918_memcpy(ADDRESS_PG+(h<<9),&PG[0][0],sizeof(PG));
	}

	for (h=0; h<4; h++) {
		uint8_t CT[64][8];
		for (cl=0; cl<8; cl++) {
			for (cr=0; cr<8; cr++) {
				for (l=0; l<8; l++) {
					CT[(cl<<3)+(cr<<0)][l] =  h?min(colors[cl][l], colors[cr][l]):colors[cl][l];
				}
			}
		}
		TMS9918_memcpy(ADDRESS_CT+(h<<9),&CT[0][0],sizeof(CT));
	}

}

void initBloodMeterTiles(uint8_t freeTiles[256]) {
	
	uint8_t i,l, v;
	U8x8 shape, color;	
	for (l=0; l<8; l++) color[l] = FLightRed;
	color[0] = FWhite;
	color[3] = FMediumRed;
	color[7] = FWhite;

	shape[0] = 0xFF;
	shape[7] = 0xFF;
	
	v = 0xFF;
	for (i=0; i<9; i++) {
		
		for (l=1; l<7; l++) 
			shape[l] = v;
			
		addASCIITile(freeTiles, 0x10+i, shape, color);
		v = v<<1;
	}
}

