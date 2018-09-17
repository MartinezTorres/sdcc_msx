#include <stdint.h>

#define TRUE 1
#define FALSE 0

uint8_t reverse8(uint8_t i);
uint16_t reverse16(uint16_t i);


#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

#define REPEAT2(a)  { {a}; {a}; }
#define REPEAT4(a)  { REPEAT2(a);  REPEAT2(a);  }
#define REPEAT8(a)  { REPEAT4(a);  REPEAT4(a);  }
#define REPEAT16(a) { REPEAT8(a);  REPEAT8(a);  }
#define REPEAT32(a) { REPEAT16(a); REPEAT16(a); }

typedef struct {
	int16_t x,y,dx,dy;
} TRect16;

typedef struct {
	uint8_t x,y;
} uint8_tp;

typedef struct {
	int16_t x,y;
} int16_tp;


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

typedef uint16_t U16x16[16];
typedef uint8_t U8x8[8];

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

	#define NOP(a) __asm nop __endasm
	#define DI(a) __asm nop __endasm
	#define EI(a) __asm nop __endasm
	
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


void TMS9918_setRegister(uint8_t reg, uint8_t val);
void TMS9918_setFlags(uint16_t flags);
void TMS9918_setMode2(uint8_t use3pages);
void TMS9918_activatePage0();
void TMS9918_activatePage1();
void TMS9918_memcpy(uint16_t dst, const uint8_t *src, uint16_t size);
void TMS9918_memset(uint16_t dst, uint8_t value, uint16_t size);

void TMS9918_waitFrame();

// 16 bit pattern manipulation functions
uint16_t *hflip16(U16x16 s);
uint16_t *rotate16(U16x16 s);
uint16_t *load16(U16x16 s, const U16x16 src);
void writeSprite16(uint8_t pos, U16x16 s);

// Text rutines
// Precondition, freeTiles must have at least 1 spot!
void getMSXROMTile(U8x8 shape, uint8_t ascii);
uint8_t addASCIITile(uint8_t freeTiles[256], uint8_t ascii, const U8x8 shape, const U8x8 color);
void uint16ToString(char *msg, uint16_t v, uint8_t enableZeroPad);
void printStr(uint16_t baseAddress, uint8_t x, uint8_t y, const char *msg);


enum { KEYBOARD_RIGHT=0x80,KEYBOARD_DOWN=0x40,KEYBOARD_UP=0x20,KEYBOARD_LEFT=0x10,KEYBOARD_DEL=0x08,KEYBOARD_INS=0x04,KEYBOARD_HOME=0x02,KEYBOARD_SPACE=0x01 } ;
uint8_t keyboard_read(void);

typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f start();


