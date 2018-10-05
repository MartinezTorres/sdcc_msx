#include <capetsrace.h>

#include <nicefonts.h>

T_f M0_menu();
extern T_f L0_level();

void printStrRAW(uint16_t baseAddress, uint8_t x, uint8_t y, const uint8_t *msg) {

	uint8_t tiles[32];
	register uint8_t l = 0;
	{
		register const uint8_t *src = msg;
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

typedef struct {uint8_t slim[32], bold[32];} TMsg; 

static void colorMsg(const TMsg *msg, uint8_t colorSlim, uint8_t colorBold) {
	
	uint8_t i;
	U8x8 color;
	for (i=0; i<8; i++) color[i] = colorSlim; 
	for (i=0; msg->slim[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->slim[i])<<3), color, 8);

	for (i=0; i<8; i++) color[i] = colorBold;
	for (i=0; msg->bold[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->bold[i])<<3), color, 8);
}

static void updateHeads(T_SA SA, uint8_t f, uint8_t selection) {

	f= f&(0xFF>>2);
	
	SA[1].pattern = (selection==1?SPRITE_HEAD_LEFT:SPRITE_HEAD_LEFT + ((f>>2)<<2));
	SA[1].color = (f&1 ? BLightRed : BWhite);

	SA[3].pattern = (selection==0?SPRITE_HEAD_RIGHT:SPRITE_HEAD_RIGHT + ((15-(f>>2))<<2));
	SA[3].color = (f&1 ? BLightYellow : BWhite);

}
/*
static void generateGuillotineTiles(uint8_t guillotine[3][8], uint8_t freeTiles[256]) {
	
	// Guillotine Top
	{
		U8x8 color, shape;
		for (i=0; i<8; i++) shape = 0xFF;
		addTile
		for (i=0; i<8; i++) color = 0xFF; 
		
	}
	uint8_t i;
	U8x8 color;
	for (i=0; i<8; i++) color[i] = colorSlim; 
	for (i=0; msg->slim[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->slim[i])<<3), color, 8);

	for (i=0; i<8; i++) color[i] = colorBold;
	for (i=0; msg->bold[i]; i++) 
		TMS9918_memcpy(ADDRESS_CT + (((uint16_t)msg->bold[i])<<3), color, 8);
}*/


T_f start() {

	TMS9918_setRegister(7,BBlack+FWhite);
	TMS9918_setMode2(FALSE);
	
	initHeadSprites(hairSprite, headSprite);
	
	
	return (T_f)(M0_menu);
}

T_f M0_menu() {	

	uint8_t freeTiles[256];
	uint8_t space;
	TMsg MchooseWisely;
	TMsg MmarieAntoniette;
	TMsg MlouisCapet;
	TMsg MraceMode;
	
	TMsg MleftTriangle, MrightTriangle;
	
	int8_t selection = 0;

	{
		uint8_t c;
		for (c = 0; c<255; c++) freeTiles[c]=1;
	}
	{
		uint8_t l;
		U8x8 shape, color;
		for (l=0; l<8; l++) {
			shape[l] = 0;
			color[l] = BBlack+FGray;
		}
		space = addASCIITile(freeTiles, ' ', shape, color);
		TMS9918_memset(ADDRESS_PN0, space, sizeof(T_PN));
		TMS9918_memset(ADDRESS_PN1, space, sizeof(T_PN));
	}

//	uint8_t guillotineTop[8], guillotineBottom[8]; 
//	generateGuillotineTiles(guillotineTop, guillotineBottom, freeTiles);
	

	initRenderedText(MchooseWisely.slim, MchooseWisely.bold, freeTiles, (const uint8_t *)"Capet's Race!", font_zelda, 1);
	initRenderedText(MmarieAntoniette.slim, MmarieAntoniette.bold, freeTiles, (const uint8_t *)"Marie Antoniette", font_zelda, 1);
	initRenderedText(MlouisCapet.slim, MlouisCapet.bold, freeTiles, (const uint8_t *)"Louis Capet", font_zelda, 3);
	initRenderedText(MraceMode.slim, MraceMode.bold, freeTiles, (const uint8_t *)"vs. Mode", font_zelda, 3);
	initRenderedText(MleftTriangle.slim, MleftTriangle.bold, freeTiles, (const uint8_t *)"\x11", font_zelda, 3);
	initRenderedText(MrightTriangle.slim, MrightTriangle.bold, freeTiles, (const uint8_t *)"\x10", font_zelda, 3);
	colorMsg (&MchooseWisely, BBlack + FWhite, BBlack + FDarkRed);
	colorMsg (&MmarieAntoniette, BBlack + FWhite, BBlack + FDarkRed);
	colorMsg (&MlouisCapet, BBlack + FWhite, BBlack + FDarkRed);
	colorMsg (&MraceMode, BBlack + FWhite, BBlack + FDarkRed);
	colorMsg (&MrightTriangle, BBlack + FWhite, BBlack + FDarkRed);
	colorMsg (&MleftTriangle, BBlack + FWhite, BBlack + FDarkRed);

	TMS9918_setFlags(TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K | TMS9918_SI | TMS9918_MAG);	

	{
		uint8_t f = 0;	
		T_SA SA;

		uint8_t spaces[2] = { space, space };
		uint8_t keyCountDown = 0;

		SA[0].x = (256/4)-16;
		SA[0].y = 50-14;
		SA[0].pattern = SPRITE_HAIR_MA;
		SA[0].color = BWhite;

		SA[1].x = (256/4)-16;
		SA[1].y = 50;
		

		SA[2].x = (3*256/4)-16;
		SA[2].y = 50-2;
		SA[2].pattern = SPRITE_HAIR_LC;
		SA[2].color = BWhite;

		SA[3].x = (3*256/4)-16;
		SA[3].y = 50;

		SA[4].y = 208;
		
		printStrRAW(ADDRESS_PN0, 12,14,MchooseWisely.slim);
		printStrRAW(ADDRESS_PN0, 11,16,MmarieAntoniette.slim);
		printStrRAW(ADDRESS_PN0, 12,17,MlouisCapet.slim);
		printStrRAW(ADDRESS_PN0, 13,18,MraceMode.slim);

		printStrRAW(ADDRESS_PN1, 12,14,MchooseWisely.bold);
		printStrRAW(ADDRESS_PN1, 11,16,MmarieAntoniette.bold);
		printStrRAW(ADDRESS_PN1, 12,17,MlouisCapet.bold);
		printStrRAW(ADDRESS_PN1, 13,18,MraceMode.bold);
		

		while (TRUE) {

			if (f&1) {
				TMS9918Status.pn10 = ADDRESS_PN0 >> 10; 
				TMS9918_writeRegister(2);
			} else {
				TMS9918Status.pn10 = ADDRESS_PN1 >> 10; 
				TMS9918_writeRegister(2);
			}
			
			f++;
			{
				uint8_t keys = keyboard_read();
				
				if (keyCountDown==0 && (keys & KEYBOARD_UP)) {

					TMS9918_memcpy(ADDRESS_PN0 + 9+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN0 +21+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN1 + 9+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN1 +21+((16+selection)<<5),spaces,2);

					if (selection)
						selection--;
					
					f = f&1;
					keyCountDown=3;
				}

				if (keyCountDown==0 && (keys & KEYBOARD_DOWN)) {

					TMS9918_memcpy(ADDRESS_PN0 + 9+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN0 +21+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN1 + 9+((16+selection)<<5),spaces,2);
					TMS9918_memcpy(ADDRESS_PN1 +21+((16+selection)<<5),spaces,2);

					if (selection!=2)
						selection++;

					f = f&1;
					keyCountDown=3;
				}

				if (keys & KEYBOARD_SPACE) {
					
					switch (selection) {
					case 0: colorMsg (&MmarieAntoniette, BBlack + FWhite, BBlack + FDarkGreen); break;
					case 1: colorMsg (&MlouisCapet, BBlack + FWhite, BBlack + FDarkGreen); break;
					case 2: colorMsg (&MraceMode, BBlack + FWhite, BBlack + FDarkGreen); break;
					}
					break;
				}
				
				if (keyCountDown && !(keys & KEYBOARD_DOWN) && !(keys & KEYBOARD_UP)) keyCountDown--;
			}
			updateHeads(SA, f, selection);


			printStrRAW(ADDRESS_PN0,  9,16+selection,MrightTriangle.slim);
			printStrRAW(ADDRESS_PN0, 21,16+selection,MleftTriangle.slim);
			printStrRAW(ADDRESS_PN1,  9,16+selection,MrightTriangle.bold);
			printStrRAW(ADDRESS_PN1, 21,16+selection,MleftTriangle.bold);

			TMS9918_memcpy(ADDRESS_SA0,(uint8_t *)SA,32);
			TMS9918_waitFrame();
		}


		{

			int16_t speedFace=0, speedHair=0;
			uint8_t tFall = 0;
			while (SA[1].y < 200 && SA[3].y < 200) {

				if (f&1) {
					TMS9918Status.pn10 = ADDRESS_PN0 >> 10; 
					TMS9918_writeRegister(2);
				} else {
					TMS9918Status.pn10 = ADDRESS_PN1 >> 10; 
					TMS9918_writeRegister(2);
				}
				
				f++;
				tFall++;

				updateHeads(SA, f, selection);
				
				if (selection!=1) {
					SA[0].y += speedHair>>8;
					SA[1].y += speedFace>>8;
				}
				if (selection!=0) {
					SA[2].y += speedHair>>8;
					SA[3].y += speedFace>>8;
				}
				
				if (tFall>3) speedHair+=0x40;
				speedFace += 0x40;

				TMS9918_memcpy(ADDRESS_SA0,(uint8_t *)SA,32);
				TMS9918_waitFrame();
			}
		}
	}
//	return (T_f)(M0_menu);

	return (T_f)(L0_level);

}
