#include <capetsrace.h>

#include <nicefonts.h>

T_f M0_menu();
extern T_f L0_levelInit();

void printStrRAW(uint16_t baseAddress, uint8_t x, uint8_t y, const char *msg) {

	uint8_t tiles[32];
	register uint8_t l = 0;
	{
		register const char *src = msg;
		register uint8_t *dst = &tiles[0];
		while (TRUE) {
			register uint8_t v = *src++;
			if (v==0) break;
			l++;
			*dst++ = v;
		}
	}
	TMS9918_memcpy(baseAddress + x+(y<<5),tiles,l);
}
/*
		for (c = 32; c<128; c++) {
			uint8_t l, v;
			U8x8 shape, color;
			for (l=0; l<8; l++) color[l] = BBlack+FGray;
			getMSXROMTile(shape, c);
			v = addASCIITile(freeTiles, c, shape, color);
		}

 */

typedef struct {uint8_t slim[32], bold[32];} TMsg; 

static void colorMsg(const TMsg *msg) {
	
	uint8_t i;
	U8x8 color;
	for (i=0; i<8; i++) color[i] = BBlack + FLightRed;
	for (i=0; msg->slim[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->slim[i])<<3), color, 8);

	for (i=0; i<8; i++) color[i] = BBlack + FGray;
	for (i=0; msg->bold[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->bold[i])<<3), color, 8);
}

T_f M0_menu() {	

	uint8_t freeTiles[256];
	uint8_t space;
	TMsg MchooseWisely;
	TMsg MmarieAntoniette;
	TMsg MlouisCapet;
	TMsg MraceMode;
	
	TMsg MleftTriangle, MrightTriangle;
	

	{
		uint8_t c;
		for (c = 0; c<255; c++) freeTiles[c]=1;
	}
	{
		uint8_t l, v;
		U8x8 shape, color;
		for (l=0; l<8; l++) {
			shape[l] = 0;
			color[l] = BBlack+FGray;
		}
		space = addASCIITile(freeTiles, ' ', shape, color);
		TMS9918_memset(ADDRESS_PN0, space, sizeof(T_PN));
	}
	

	initRenderedText(MchooseWisely.slim, MchooseWisely.bold, freeTiles, "Capet's Race!", font_zelda, 1);
	initRenderedText(MmarieAntoniette.slim, MmarieAntoniette.bold, freeTiles, "Marie Antoniette", font_zelda, 1);
	initRenderedText(MlouisCapet.slim, MlouisCapet.bold, freeTiles, "Louis Capet", font_zelda, 3);
	initRenderedText(MraceMode.slim, MraceMode.bold, freeTiles, "vs. Mode", font_zelda, 3);
	initRenderedText(MleftTriangle.slim, MleftTriangle.bold, freeTiles, "\x11", font_zelda, 3);
	initRenderedText(MrightTriangle.slim, MrightTriangle.bold, freeTiles, "\x10", font_zelda, 3);
	colorMsg (&MchooseWisely);
	colorMsg (&MmarieAntoniette);
	colorMsg (&MlouisCapet);
	colorMsg (&MraceMode);
	colorMsg (&MrightTriangle);
	colorMsg (&MleftTriangle);

	TMS9918_setFlags(TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K | TMS9918_SI | TMS9918_MAG);	

	{
		int8_t selection = 0;
		T_SA SA;
		uint8_t f = 0;	
		while (TRUE) {
			
			SA[0].x = (256/4)-16;
			SA[0].y = 50-14;
			SA[0].pattern = SPRITE_HAIR_MA;
			SA[0].color = BWhite;

			SA[1].x = (256/4)-16;
			SA[1].y = 50;
			SA[1].pattern = SPRITE_HEAD_LEFT +((f>>2)<<2);
			SA[1].color = BLightRed;
			if (f&1) SA[1].color = BWhite;
			if (selection==1) SA[1].pattern = SPRITE_HEAD_LEFT;
			

			SA[2].x = (3*256/4)-16;
			SA[2].y = 50-2;
			SA[2].pattern = SPRITE_HAIR_LC;
			SA[2].color = BWhite;

			SA[3].x = (3*256/4)-16;
			SA[3].y = 50;
			SA[3].pattern = SPRITE_HEAD_RIGHT + ((15-(f>>2))<<2);
			SA[3].color = BLightYellow;
			if (f&1) SA[3].color = BWhite;
			if (selection==0) SA[3].pattern = SPRITE_HEAD_RIGHT;

			SA[4].y = 208;
			
			f++;
			f= f&(0xFF>>2);
			
			//TMS9918_memset(ADDRESS_PN0, space, sizeof(T_PN));

			if (f&1){
				printStrRAW(ADDRESS_PN0, 12,14,MchooseWisely.slim);
				printStrRAW(ADDRESS_PN0, 11,16,MmarieAntoniette.slim);
				printStrRAW(ADDRESS_PN0, 12,17,MlouisCapet.slim);
				printStrRAW(ADDRESS_PN0, 13,18,MraceMode.slim);
				
				printStrRAW(ADDRESS_PN0,  9,16+selection,MrightTriangle.slim);
				printStrRAW(ADDRESS_PN0, 21,16+selection,MleftTriangle.slim);

			} else {
				printStrRAW(ADDRESS_PN0, 12,14,MchooseWisely.bold);
				printStrRAW(ADDRESS_PN0, 11,16,MmarieAntoniette.bold);
				printStrRAW(ADDRESS_PN0, 12,17,MlouisCapet.bold);
				printStrRAW(ADDRESS_PN0, 13,18,MraceMode.bold);
				
				printStrRAW(ADDRESS_PN0,  9,16+selection,MrightTriangle.bold);
				printStrRAW(ADDRESS_PN0, 21,16+selection,MleftTriangle.bold);
			}

			{
				uint8_t keys = keyboard_read();
				
				if (keys & KEYBOARD_UP) {
					if (!selection) 
						selection = 2;
					else
						selection--;
				}

				if (keys & KEYBOARD_DOWN) {
					if (selection==2) 
						selection = 0;
					else
						selection++;
				}

				if (keys & KEYBOARD_SPACE) {
				}
			}
			
			TMS9918_memcpy(ADDRESS_SA0,(uint8_t *)SA,32);
			TMS9918_waitFrame();
		}
	}

	//return (T_f)(L0_levelInit);
}
