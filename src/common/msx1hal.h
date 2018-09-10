#include <stdint.h>

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
	union {
		uint8_t reg[8];
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

void setTMS9918_setMode2(); // Configured for a single 256 character table.
void setTMS9918_activatePage0();
void setTMS9918_activatePage1();

void setTMS9918_setRegister(uint8_t reg, uint8_t val);
void setTMS9918_write(uint16_t dst, uint8_t *src, uint16_t sz);


enum { KEYBOARD_RIGHT=0x80,KEYBOARD_DOWN=0x40,KEYBOARD_UP=0x20,KEYBOARD_LEFT=0x10,KEYBOARD_DEL=0x08,KEYBOARD_INS=0x04,KEYBOARD_HOME=0x02,KEYBOARD_SPACE=0x01 } ;
uint8_t keyboard_read(void);

typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f start();


