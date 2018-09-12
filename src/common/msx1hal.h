#include <stdint.h>

#ifdef MSX
	#define printf(...)
#elif LINUX
	#include <stdio.h>
#else
	#error "Architecture Not Supported"
#endif

enum {
BTransparent=0x0,
BBlack,
BMediumGreen,
BLightGreen,
BDarkBlue,  	
BLightBlue,
BDarkRed,
BCyan,
BMediumRed,
BLightRed,
BDarkYellow,
BLightYellow,
BDarkGreen,
BMagenta,
BGray,
BWhite};

enum {
FTransparent=0x00,
FBlack=0x10,
FMediumGreen=0x20,
FLightGreen=0x30,
FDarkBlue=0x40,
FLightBlue=0x50,
FDarkRed=0x60,
FCyan=0x70,
FMediumRed=0x80,	
FLightRed=0x90,
FDarkYellow=0xA0,
FLightYellow=0xB0,
FDarkGreen=0xC0,
FMagenta=0xD0,
FGray=0xE0,
FWhite=0xF0};

// Memory Map M2
// PG: 0x0000-0x17FF
// PN0: 0x1800-0x1AFF
// SA0: 0x1B00-0x1B7F
// PN1: 0x1B00-0x1EFF
// SA1: 0x1F00-0x1F7F
// CT: 0x2000-0x37FF
// SP: 0x3800-0x3FFF

#define ADDRESS_PN0 0x1800
#define ADDRESS_PN1 0x1B00
#define ADDRESS_CT  0x2000
#define ADDRESS_PG  0x0000
#define ADDRESS_SA0 0x1F00
#define ADDRESS_SA1 0x1F80
#define ADDRESS_SG  0x3800


typedef struct {
	uint8_t y,x;
	uint8_t pattern;
	uint8_t color;
} Sprite;

#define N_SPRITES 32
#define TILE_WIDTH 32
#define TILE_HEIGHT 24
typedef uint8_t T_PN[TILE_HEIGHT][TILE_WIDTH]; // Pattern Name Table
typedef uint8_t T_CT[3][256][8]; // Pattern Color Table
typedef uint8_t T_PG[3][256][8]; // Pattern Generator Table
typedef Sprite  T_SA[N_SPRITES]; // Sprite Attribute Table
typedef uint8_t T_SG[256][8];    // Sprite Generator Table

typedef uint8_t Tile[8][2];


typedef struct {
	union {
		uint8_t reg[8];
		uint16_t flags;
		struct {
			struct {
				uint8_t extvid : 1;		
				uint8_t mode2 : 1;
				uint8_t reserved1 : 6;
			};
			struct {
				uint8_t magnifySprites : 1;
				uint8_t sprites16 : 1;
				uint8_t reserved2: 1;
				uint8_t mode3 : 1;
				uint8_t mode1 : 1;
				uint8_t generateInterrupts : 1;
				uint8_t blankScreen : 1;
				uint8_t mem416K : 1;
			};
			uint8_t pn10, ct6, pg11, sa7, sg11;
			struct {
				uint8_t backdrop : 4;
				uint8_t textcolor : 4;
			};
		};
	};
} TMS9918Register;

enum  {
TMS9918_M2 = 0x0002,
TMS9918_EXTVID = 0x0001,
TMS9918_MEM416K = 0x8000,
TMS9918_BLANK = 0x4000,
TMS9918_GINT = 0x2000,
TMS9918_M1 = 0x1000,
TMS9918_M3 = 0x0800,
TMS9918_SI = 0x0200,
TMS9918_MAG = 0x0100,
};


#ifdef MSX

	inline static void NOP(void) { __asm nop __endasm; }
	inline static void DI (void) { __asm di __endasm; }
	inline static void EI (void) { __asm ei __endasm; }

	__sfr __at 0x98 VDP0;
	__sfr __at 0x99 VDP1;

	TMS9918Register __at 0xF3DF TMS9918Status;

	inline static void TMS9918_writeRegister(uint8_t reg) {

		VDP1 = TMS9918Status.reg[reg];
		NOP();
		VDP1 = 0x80 | reg;
		NOP();
	}

#elif LINUX
	extern TMS9918Register TMS9918Status;

	inline static void TMS9918_writeRegister(uint8_t reg) { (void)reg;};
#else
	#error "Architecture Not Supported"
#endif

inline static void TMS9918_setRegister(uint8_t reg, uint8_t val) {
	
	TMS9918Status.reg[reg] = val; 
	TMS9918_writeRegister(reg);
}

inline static void TMS9918_setFlags(uint16_t flags) {
	
	TMS9918Status.flags = flags; 
	TMS9918_writeRegister(0);
	TMS9918_writeRegister(1);
}

inline static void TMS9918_setMode2() {

	TMS9918Status.flags = TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K;
	
	TMS9918Status.pn10 =  ADDRESS_PN0 >> 10;
	TMS9918Status.ct6  = (ADDRESS_CT  >>  6) | 0b01111111;
	TMS9918Status.pg11 = (((int16_t)ADDRESS_PG)  >> 11) | 0b00000011;
	TMS9918Status.sa7  =  ADDRESS_SA0 >>  7;
	TMS9918Status.sg11 =  ADDRESS_SG  >> 11;
	
	TMS9918Status.backdrop  = BBlack;
	TMS9918Status.textcolor = BWhite;
	
	{
		uint8_t i=0;
		for (i=0; i<8; ++i)
			TMS9918_writeRegister(i);
	}	
}

inline static void TMS9918_activatePage0() { 
	
	TMS9918Status.pn10 = ADDRESS_PN0 >> 10; TMS9918_writeRegister(2);
	TMS9918Status.sa7  = ADDRESS_SA0 >>  7; TMS9918_writeRegister(5);
}

inline static void TMS9918_activatePage1() {
	
	TMS9918Status.pn10 = ADDRESS_PN1 >> 10; TMS9918_writeRegister(2);
	TMS9918Status.sa7  = ADDRESS_SA1 >>  7; TMS9918_writeRegister(5);
}


void TMS9918_write(uint16_t dst, const uint8_t *src, uint16_t sz);
void TMS9918_write8(uint16_t dst, const uint8_t *src, uint8_t sz8);

void TMS9918_waitFrame();


enum { KEYBOARD_RIGHT=0x80,KEYBOARD_DOWN=0x40,KEYBOARD_UP=0x20,KEYBOARD_LEFT=0x10,KEYBOARD_DEL=0x08,KEYBOARD_INS=0x04,KEYBOARD_HOME=0x02,KEYBOARD_SPACE=0x01 } ;
uint8_t keyboard_read(void);

typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f start();


