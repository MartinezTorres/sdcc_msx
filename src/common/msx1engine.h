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
/*typedef struct {
	enum { MODE0=0, MODE1=1, MODE2=2 } mode;
	enum { SPRITE8X8=0, SPRITE16X16=1 } spriteSize;
	uint8_t isBlank;
	uint8_t generateInterrupts;
	uint8_t magnifySprites;
	uint8_t textcolor, backdrop;
	
	uint16_t PG;
	uint16_t PN;
	uint16_t SA;
	uint16_t CT;
	uint16_t SP;
} TMS9918Settings;*/

//void setTMS9918_setRegister();
void setTMS9918_activatePage0();
void setTMS9918_activatePage1();
void setTMS9918_write(uint16_t dst, uint8_t *src, uint8_t sz);

enum { KEYBOARD_RIGHT=0x80,KEYBOARD_DOWN=0x40,KEYBOARD_UP=0x20,KEYBOARD_LEFT=0x10,KEYBOARD_DEL=0x08,KEYBOARD_INS=0x04,KEYBOARD_HOME=0x02,KEYBOARD_SPACE=0x01 } ;
uint8_t keyboard_read(void);

typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f start();


