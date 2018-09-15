#include <stdint.h>
#include <msx1hal.h>
#include <util.h>

#include <SDL2/SDL.h>
#include "font8x8_basic.h"


// VDP EMULATOR
#define N_SPRITES 32
#define TILE_WIDTH 32
#define TILE_HEIGHT 24
#define TEX_WIDTH (TILE_WIDTH*8)
#define TEX_HEIGHT (TILE_HEIGHT*8)
#define SCREEN_WIDTH (TEX_WIDTH*4)
#define SCREEN_HEIGHT (TEX_HEIGHT*4)

typedef struct { uint8_t r,g,b; } RGB;
RGB framebuffer[TEX_HEIGHT][TEX_WIDTH];

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

TMS9918Register TMS9918Status;
uint8_t TMS9918VRAM[0x4000];

static inline void drawMode2(const T_PN PN, const T_CT CT, const T_PG PG, const T_SA SA, const T_SG SG) {
	
	// TILES
	for (int i=0; i<TILE_HEIGHT; i++) {
		for (int j=0; j<TILE_WIDTH; j++) {
			for (int ii=0; ii<8; ii++) {
				uint8_t p = PG[i/8][PN[i][j]][ii];
				uint8_t c = CT[i/8][PN[i][j]][ii];
				RGB *pix = &framebuffer[i*8+ii][j*8];
				for (int jj=0; jj<8; jj++) {
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
		
		int maxSprite;
		{
			int nShownSprites=0;

			for (maxSprite=0; maxSprite<N_SPRITES && SA[maxSprite].y!=208 && nShownSprites<4; maxSprite++) {

				uint8_t spriteLine = (i-SA[maxSprite].y-1) >> TMS9918Status.magnifySprites;
				
				if (spriteLine>=8 * (1+TMS9918Status.sprites16)) continue;
				nShownSprites++;
			}
		}


		for (int j=maxSprite; j>=0; j--) {

			uint8_t spriteLine = (i-SA[j].y-1) >> TMS9918Status.magnifySprites;
			
			if (spriteLine>=8 * (1+TMS9918Status.sprites16)) continue;
			
			uint8_t pattern = SA[j].pattern;			
			if (TMS9918Status.sprites16) pattern = (pattern & 252) + !!(spriteLine>7);

			int y = i;
			int x = SA[j].x - (32*!!(SA[j].color&128));

			for (int k=0; k<=TMS9918Status.sprites16; k++) {

				uint8_t p = SG[pattern+2*k][spriteLine%8];
				for (int jj=0; jj<8; jj++) {
					
					for (int m=0; m<=TMS9918Status.magnifySprites; m++) {

						if (x>=0 && x<TILE_WIDTH*8 && (p&128) && (SA[j].color&0xF))
							framebuffer[y][x]=colors[SA[j].color&0xF];
						x++;
					}
					p*=2;
				}
			}
		}
	}
}

static inline void drawScreen() {

	for (int i=0; i<TEX_HEIGHT; i++)
		for (int j=0; j<TEX_WIDTH; j++)
			framebuffer[i][j] = colors[TMS9918Status.backdrop];
			
	if (! TMS9918Status.blankScreen) return;

	if (TMS9918Status.mode2) {
		const T_PN *PN = (T_PN *)&TMS9918VRAM[(uint16_t)(TMS9918Status.pn10)<<10];
		const T_CT *CT = (T_CT *)&TMS9918VRAM[(uint16_t)(TMS9918Status.ct6&0x80)<< 6];
		const T_PG *PG = (T_PG *)&TMS9918VRAM[(uint16_t)(TMS9918Status.pg11&0xFC)<<11];
		const T_SA *SA = (T_SA *)&TMS9918VRAM[(uint16_t)(TMS9918Status.sa7 )<< 7];
		const T_SG *SG = (T_SG *)&TMS9918VRAM[(uint16_t)(TMS9918Status.sg11)<<11];

		drawMode2(*PN, *CT, *PG, *SA, *SG); //only mode2 is supported
	}
}


// Memory Map M2
// PG: 0x0000-0x17FF
// PN0: 0x1800-0x1AFF
// SA0: 0x1F00-0x1B7F
// PN1: 0x1B00-0x1EFF
// SA1: 0x1F10-0x1FFF
// CT: 0x2000-0x37FF
// SG: 0x3800-0x3FFF

void TMS9918_write(uint16_t dst, const uint8_t *src, uint16_t sz) {
	
	memcpy(&TMS9918VRAM[dst], src, sz);
}

void TMS9918_write8(uint16_t dst, const uint8_t *src, uint8_t sz8) {
	
	memcpy(&TMS9918VRAM[dst], src, sz8*8);
}

// KEYBOARD
static uint8_t keyboardStatus;
static uint8_t keys[8831];
static inline void keyboard_init(void) {
	
	keyboardStatus = 0;
	
	memset(keys, 0, sizeof(keys));
	keys[SDLK_RIGHT %sizeof(keys)] = KEYBOARD_RIGHT;
	keys[SDLK_DOWN  %sizeof(keys)] = KEYBOARD_DOWN;
	keys[SDLK_UP    %sizeof(keys)] = KEYBOARD_UP;
	keys[SDLK_LEFT  %sizeof(keys)] = KEYBOARD_LEFT;
	keys[SDLK_DELETE%sizeof(keys)] = KEYBOARD_DEL;
	keys[SDLK_INSERT%sizeof(keys)] = KEYBOARD_INS;
	keys[SDLK_HOME  %sizeof(keys)] = KEYBOARD_HOME;
	keys[SDLK_SPACE %sizeof(keys)] = KEYBOARD_SPACE;
}

uint8_t keyboard_read(void) { return keyboardStatus; }


// SDL BACKEND
static SDL_Window* gWindow = NULL;
static SDL_Renderer* gRenderer = NULL;
static SDL_Texture* tex = NULL;

static inline int8_t initSDL() {
	    
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

    return 0;
}

static inline int8_t displayFramebufferSDL() {
	
	
		// clear screen
		SDL_RenderClear(gRenderer);
		
		void *mPixels;
		int mPitch;

		//Lock texture for manipulation
		SDL_LockTexture( tex, NULL, &mPixels, &mPitch );
				
		//Copy loaded/formatted surface pixels
		memcpy( mPixels, framebuffer, sizeof(framebuffer));

		//Unlock texture to update
		SDL_UnlockTexture( tex );
		
		SDL_RenderCopy(gRenderer, tex,NULL,NULL);
		// update screen
		SDL_RenderPresent(gRenderer);
		
		return 0;
}

static inline void closeSDL() {
	
	SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    SDL_Quit();
}


void TMS9918_waitFrame() {

	SDL_Event e;
	// handle event on queue
	while (SDL_PollEvent(&e) != 0) {
		
		switch( e.type ){
			
			case SDL_QUIT:
				closeSDL();
				return ;
			case SDL_KEYDOWN:
				//printf("KEY PRESSED: %d\n",e.key.keysym.sym);
				keyboardStatus = keyboardStatus | keys[e.key.keysym.sym%sizeof(keys)];
				break;
			case SDL_KEYUP:
				//printf("KEY RELEASED: %d\n",e.key.keysym.sym);
				keyboardStatus = keyboardStatus & ~keys[e.key.keysym.sym%sizeof(keys)];
				break;
		}
	}
	
	drawScreen();
	
	displayFramebufferSDL();
	
	uint32_t delay = SDL_GetTicks()%(1000/60);
	SDL_Delay(1000/60-delay);
}


int main() {
		
	T_f state_ptr = (T_f)start;
	
	if (initSDL()<0) {
		printf("Failed to initialize SDL!\n");
		return -1;
	}

	keyboard_init();
	
	while (TRUE) {
		state_ptr = (T_f)((*state_ptr)());
		TMS9918_waitFrame();
	}    
}

