#include <msx1hal.h>

enum { 
	SPRITE_HAIR_MA = 0x00, 
	SPRITE_HAIR_LC = 0x04, 
	SPRITE_HEAD_RIGHT = 0x08,
	SPRITE_HEAD_LEFT = 0x48,
};

enum { T_PLAYER };
enum { ST_RESTING, ST_JUMP0, ST_JUMP1, ST_JUMP2 };
enum { LEFT=0x1, RIGHT=0x2, TOP=0x4, BOTTOM=0x8};



void initHeadSprites(const U16x16 hair[2], const U16x16 face[4]);
void initMapTiles(const U8x8 shapes[8], const U8x8 colors[8]);
void initBloodMeterTiles(uint8_t freeTiles[256]);
