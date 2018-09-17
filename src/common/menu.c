#include <capetsrace.h>

T_f M0_menu();
extern T_f L0_levelInit();


T_f M0_menu() {	


	TMS9918_setFlags(TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K | TMS9918_SI | TMS9918_MAG);	

	{
		uint8_t freeTiles[256];
		uint8_t c;
		for (c = 0; c<255; c++) freeTiles[c]=1;
		for (c = 32; c<128; c++) {
			uint8_t l, v;
			U8x8 shape, color;
			for (l=0; l<8; l++) color[l] = BBlack+FGray;
			getMSXROMTile(shape, c);

			v = addASCIITile(freeTiles, c, shape, color);
			//printf("%d\n", v);
			if (c==32)
				TMS9918_memset(ADDRESS_PN0, v, sizeof(T_PN));
		}
		printStr(ADDRESS_PN0, 5,17,"Choose Wisely:");
	}

	{
		T_SA SA;
		uint8_t f = 0;	
		while (TRUE) {
			SA[0].x = (256/3)-16;
			SA[0].y = 80-14;
			SA[0].pattern = SPRITE_HAIR_MA;
			SA[0].color = BWhite;

			SA[1].x = (256/3)-16;
			SA[1].y = 80;
			SA[1].pattern = SPRITE_HEAD_LEFT +((f>>2)<<2);
			SA[1].color = BLightRed;
			if (f&1) SA[1].color = BWhite;

			SA[2].x = (2*256/3)-16;
			SA[2].y = 80-2;
			SA[2].pattern = SPRITE_HAIR_LC;
			SA[2].color = BWhite;

			SA[3].x = (2*256/3)-16;
			SA[3].y = 80;
			SA[3].pattern = SPRITE_HEAD_RIGHT + ((15-(f>>2))<<2);
			SA[3].color = BLightYellow;
			if (f&1) SA[3].color = BWhite;


			SA[4].y = 208;
			
			f++;
			f= f&(0xFF>>2);
			
			TMS9918_memcpy(ADDRESS_SA0,(uint8_t *)SA,32);
			TMS9918_waitFrame();
		}
	}

	//return (T_f)(L0_levelInit);
}
