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


typedef struct {
	enum { MODE0=0, MODE1=1, MODE2=2 } mode;
	enum { SPRITE8X8=0, SPRITE16X16=1 } spriteSize;
	uint8_t isBlank;
	uint8_t generateInterrupts;
	uint8_t magnifySprites;
} TMS9918Settings;

void setTMS9918_setSettings(TMS9918Settings &mode);
void setTMS9918_setMode2Page0();
void setTMS9918_setMode2Page1();



enum { KEYBOARD_RIGHT=0x80,KEYBOARD_DOWN=0x40,KEYBOARD_UP=0x20,KEYBOARD_LEFT=0x10,KEYBOARD_DEL=0x08,KEYBOARD_INS=0x04,KEYBOARD_HOME=0x02,KEYBOARD_SPACE=0x01 } ;
uint8_t keyboard_read(void);


typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f init();
T_f state_ptr;

enum { KEY_RIGHT=SDLK_RIGHT, KEY_LEFT=SDLK_LEFT, KEY_DOWN=SDLK_DOWN, KEY_UP=SDLK_UP, KEY_SPACE=' ' };

int8_t mainLoop();


