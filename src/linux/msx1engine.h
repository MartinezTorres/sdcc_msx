#include <SDL2/SDL.h>

#include "font8x8_basic.h"

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

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* tex = NULL;

static uint8_t reverse8(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

// initialize sdl
static int8_t initSDL() {
	    
    // Intialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        //std::cout << "SDL could not initialize! error: " << SDL_GetError() << "\n";
        return -1;
	}

	// create window
	gWindow = SDL_CreateWindow("SDL Skeleton", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		//std::cout << "Window could not be created! error: " << SDL_GetError() << "\n";
        return -2;
	}
	// create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) {
		//std::cout << "Renderer could not be created. error: " << SDL_GetError() << "\n";
        return -3;
	}

	SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
	tex = SDL_CreateTexture(gRenderer,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,TEX_WIDTH,TEX_HEIGHT);

	memcpy(GT[0],font8x8_basic,sizeof(font8x8_basic));
	memcpy(GT[1],font8x8_basic,sizeof(font8x8_basic));
	memcpy(GT[2],font8x8_basic,sizeof(font8x8_basic));
	for (uint i=0; i<sizeof(GT); i++) {
		((uint8_t *)GT)[i] = reverse8(((uint8_t *)GT)[i]);
	}
	
	memset(CT[0],BBlack+FWhite,sizeof(CT[0]));
	memset(CT[1],BBlack+FWhite,sizeof(CT[1]));
	memset(CT[2],BBlack+FWhite,sizeof(CT[2]));
	
    return 0;
}

void closeSDL() {
	
	SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    SDL_Quit();
}

RGB framebuffer[TEX_HEIGHT][TEX_WIDTH];

uint8_t keys[256];


typedef void (*T_void)(void);
typedef T_void (*T_f)(void);
T_f I0_init();
T_f state_ptr;
int8_t updateLoop() { state_ptr = (T_f)((*state_ptr)()); return 0;}


int8_t mainLoop() {

	printf("aa\n");
	
	if (initSDL()<0) {
		printf("Failed to initialize SDL!\n");
		return -1;
	}
			
	while (updateLoop()>=0) {
		

		
		SDL_Event e;
		// handle event on queue
		while (SDL_PollEvent(&e) != 0) {
			
			switch( e.type ){
				
				case SDL_QUIT:
					return 0;
				case SDL_KEYDOWN:
					//std::cout << "UP: " << e.key.keysym.sym%256 << std::endl;
					keys[e.key.keysym.sym%256] = 0x01;
					break;
                case SDL_KEYUP:
					//std::cout << "DOWN: " << e.key.keysym.sym%256 << std::endl;
					keys[e.key.keysym.sym%256] = 0x00;
                    break;
			}
		}
		
		uint32_t ticksStart = SDL_GetTicks();

		// clear screen
		SDL_RenderClear(gRenderer);
		
		void *mPixels;
		int mPitch;

		//Lock texture for manipulation
		SDL_LockTexture( tex, NULL, &mPixels, &mPitch );
		
		// TILES
		for (int i=0; i<TILE_HEIGHT; i++) {
			for (int j=0; j<TILE_WIDTH; j++) {
				for (int ii=0; ii<8; ii++) {
					uint8_t p = GT[i/8][PN[i][j]][ii];
					uint8_t c = CT[i/8][PN[i][j]][ii];
					RGB *pix = &framebuffer[i*8+ii][j*8];
					for (int jj=0; jj<8; jj++) {
						pix[jj] = colors[BD];
						if (p&128) {
							if (c>>4) {
								pix[jj]=colors[c>>4];
							}
						} else {
							if (c & 0xF) {
								pix[jj]=colors[c&0xF];
							}
						}
						p*=2;
					}
				}
			}
		}
		
		// SPRITES
		for (int i=0; i<TILE_HEIGHT*8; i++) {
			int nShownSprites=0;
			for (int j=0; j<N_SPRITES && SA[j].y!=208 && nShownSprites<4; j++) {
				uint8_t spriteLine = i-SA[j].y-1;
				if (spriteLine>7) continue;
				nShownSprites++;
				
				uint8_t p = SG[SA[j].pattern][spriteLine];
				for (int jj=0; jj<8; jj++) {
					int xCoord = SA[j].x - (32*!!(SA[j].color&128)) + jj;
					if (xCoord<0 || xCoord>=TILE_WIDTH*8) continue;
					RGB *pix = &framebuffer[i][xCoord];
					if (p&128) {
						if (SA[j].color&0xF) {
							*pix=colors[SA[j].color&0xF];
						}
					}
					p*=2;
				}
			}
		}
		
		
		//Copy loaded/formatted surface pixels
		memcpy( mPixels, framebuffer, sizeof(framebuffer));

		//Unlock texture to update
		SDL_UnlockTexture( tex );
		
		SDL_RenderCopy(gRenderer, tex,NULL,NULL);
		// update screen
		SDL_RenderPresent(gRenderer);
		
		//std::cout << "Frame! (" << (SDL_GetTicks()-ticksStart) << ")" << std::endl;
		
		uint32_t delay = SDL_GetTicks()-ticksStart;
		if (delay>(1000/60-1)) delay = (1000/60-1);
		SDL_Delay(1000/60-delay);
	}    
	closeSDL();
	return 0;
}

int main() {
	
	state_ptr = (T_f)(I0_init);
	return mainLoop();
}

