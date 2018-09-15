#include <msx1hal.h>
#include <util.h>

#include <res/sprites.h>
#include <res/asciitiles.h>

#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

T_f M0_menu();
extern T_f L0_levelInit();

static uint16_t *hflip16(uint16_t *s) {
	
	uint8_t i=0;
	for (i=0; i<16; i++)
		s[i] = reverse16(s[i]);
		
	return s;
}

static uint16_t *rotate16(uint16_t *s) {

	uint16_t tmp[16];
	register uint16_t i,j;
	register uint16_t *src, *tgt;

	
	src = &s[15];
	for (i=0; i<16; i++) {
		register uint16_t sv = *src--;
		tgt = &tmp[15];
		for (j=0; j<16; j++) {
			register uint16_t t = *(uint16_t *)tgt;
			t += t + (sv&1);
			sv = sv>>1;
			*(uint16_t *)tgt = t;
			tgt--;
		}
	}

	src = tmp;
	tgt = s;
	for (i=0; i<16; i++) 
		*tgt++ = *src++;
	
	return s;
}

static uint16_t *shuffle16(uint16_t *s) {

	uint8_t tmp[32];
	register uint8_t i;
	register uint8_t *src, *tgt;

	tgt = ((uint8_t *)tmp)+0;
	src = ((uint8_t *)s)+1;
	for (i=0; i<16; i++) {
		*tgt = *src;
		tgt++; src+=2;
	}

	src = ((uint8_t *)s)+0;
	for (i=0; i<16; i++) {
		*tgt = *src;
		tgt++; src+=2;
	}

	tgt = ((uint8_t *)s)+0;
	src = ((uint8_t *)tmp)+0;
	for (i=0; i<32; i++) 
		*tgt++ = *src++;

	return s;
}

static uint16_t *load16(uint16_t *s, const uint16_t *cs) {
	
	register uint8_t *s_ = (uint8_t *)s;
	register const uint8_t *cs_ = (const uint8_t *)cs;
	uint8_t i;
	for (i=0; i<32; i++) 
		*s_++ = *cs_++;
		
	return s;
}

static inline void writeSprite16(uint16_t pos, uint16_t *s) {
	shuffle16(s);
	TMS9918_write8(ADDRESS_SG+(pos<<3), (const uint8_t *)s, 32/8);
}

enum { 
	SPRITE_HAIR_MA = 0x00, 
	SPRITE_HAIR_LC = 0x04, 
	SPRITE_HEAD_RIGHT = 0x08,
	SPRITE_HEAD_LEFT = 0x48,
};

static void loadHeadSprites() {
	
	uint16_t tmp[16];
	uint8_t i, j, k;
	TMS9918_setFlags(TMS9918_M2 | TMS9918_GINT | TMS9918_MEM416K);
	
	writeSprite16(SPRITE_HAIR_MA,load16(tmp,hairSprite[0]));
	writeSprite16(SPRITE_HAIR_LC,load16(tmp,hairSprite[1]));
	for (k=0; k<2; k++) {
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				uint8_t jj=j;
				load16(tmp,headSprite[i]);
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

static T_SA SA;

T_f M0_menu() {	
	
	uint8_t f = 0;
	
	TMS9918_setMode2();
	TMS9918_setRegister(7,BBlack+FWhite);

	loadHeadSprites();

	TMS9918_setFlags(TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K | TMS9918_SI | TMS9918_MAG);	
	
	
	while (TRUE) {
		SA[0].x = (256/3)-16;
		SA[0].y = 100-14;
		SA[0].pattern = SPRITE_HAIR_MA;
		SA[0].color = BWhite;

		SA[1].x = (256/3)-16;
		SA[1].y = 100;
		SA[1].pattern = SPRITE_HEAD_LEFT +((f>>1)<<2);
		SA[1].color = BLightRed;
		if (f&1) SA[1].color = BWhite;

		SA[2].x = (2*256/3)-16;
		SA[2].y = 100-2;
		SA[2].pattern = SPRITE_HAIR_LC;
		SA[2].color = BWhite;

		SA[3].x = (2*256/3)-16;
		SA[3].y = 100;
		SA[3].pattern = SPRITE_HEAD_RIGHT + ((15-(f>>1))<<2);
		SA[3].color = BLightYellow;
		if (f&1) SA[3].color = BWhite;


		SA[4].y = 208;
		
		f++;
		f= f&0x1F;
		
		TMS9918_write8(ADDRESS_SA0,(uint8_t *)SA,4);
		TMS9918_waitFrame();
	}

	return (T_f)(L0_levelInit);
}
