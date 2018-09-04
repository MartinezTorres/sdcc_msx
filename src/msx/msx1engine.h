#include <types.h>

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
BWhite,
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
	uint8_t r,g,b;
} RGB;

const RGB colors[16] = {
{   0,    0,    0},
{   0,    0,    0},
{  33,  200,   66},
{  94,  220,  120},
{  84,   85,  237},
{ 125,  118,  252},
{ 212,   82,   77},
{  66,  235,  245},
{ 252,   85,   84},
{ 255,  121,  120},
{ 212,  193,   84},
{ 230,  206,  128},
{  33,  176,   59},
{ 201,   91,  186},
{ 204,  204,  204},
{ 255,  255,  255}	
};

#define N_SPRITES 32
#define TILE_WIDTH 32
#define TILE_HEIGHT 24

uint8_t BD=BDarkBlue;

typedef struct {
	uint8_t x,y;
	uint8_t pattern;
	uint8_t color;
} Sprite;

Sprite SA[N_SPRITES]; // Sprite Attribute
uint8_t SG[256][8];

uint8_t PN[TILE_HEIGHT][TILE_WIDTH]; // Pattern Name Table
uint8_t GT[3][256][8]; // Pattern Generator Table
uint8_t CT[3][256][8]; // Pattern Color Table


// screen dimension constants
#define TEX_WIDTH (TILE_WIDTH*8)
#define TEX_HEIGHT (TILE_HEIGHT*8)
#define SCREEN_WIDTH (TEX_WIDTH*4)
#define SCREEN_HEIGHT (TEX_HEIGHT*4)

RGB framebuffer[TEX_HEIGHT][TEX_WIDTH];

uint8_t keys[256];

typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f I0_init();
T_f state_ptr;
int8_t updateLoop() { state_ptr = (T_f)((*state_ptr)()); return 0;}


int8_t mainLoop() {
	
			
	while (updateLoop()>=0) {}
	return 0;
}

int main() {
	
	state_ptr = (T_f)(I0_init);
	return mainLoop();
}

