#include <stdint.h>
#include <msx1engine.h>
#include <util.h>

#include <SDL2/SDL.h>
#include "font8x8_basic.h"

typedef struct { uint8_t r,g,b; } RGB;

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

struct {
	
	uint8_t ram[0x4000];
	
	union {
		uint8_t reg[8];
		struct {
			struct {
				uint8_t extvid : 1;		
				uint8_t m2 : 1;
				uint8_t reserved1 : 6;
			};
			struct {
				uint8_t mag : 1;
				uint8_t si : 1;
				uint8_t reserved2: 1;
				uint8_t m3 : 1;
				uint8_t m1 : 1;
				uint8_t gint : 1;
				uint8_t bl : 1;
				uint8_t mem416K : 1;
			};
			uint8_t pn10, ct6, pg11, sa7, sg11;
			struct {
				uint8_t bd : 4;
				uint8_t tc : 4;
			};
		};
	};
} TMS9918Status;

// KEYBOARD
uint8_t keyboardStatus;
uint8_t keys[8831];
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


// SDL GRAPHICAL BACKEND

#define N_SPRITES 32
#define TILE_WIDTH 32
#define TILE_HEIGHT 24
#define TEX_WIDTH (TILE_WIDTH*8)
#define TEX_HEIGHT (TILE_HEIGHT*8)
#define SCREEN_WIDTH (TEX_WIDTH*4)
#define SCREEN_HEIGHT (TEX_HEIGHT*4)

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* tex = NULL;
RGB framebuffer[TEX_HEIGHT][TEX_WIDTH];

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

static inline int8_t drawFramebufferSDL() {
	
	
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


int main() {
	
	memset(&TMS9918Status,0,sizeof(TMS9918Status));
	for (int i=0; i<8; i++) printf("%d: %02X\n",i,TMS9918Status.reg[i]);		
	TMS9918Status.m2 = 1;
	TMS9918Status.bd = 0xA;
	for (int i=0; i<8; i++) printf("%d: %02X\n",i,TMS9918Status.reg[i]);
	
	
	T_f	state_ptr = (T_f)(start);
	
	if (initSDL()<0) {
		printf("Failed to initialize SDL!\n");
		return -1;
	}

	keyboard_init();
	
	while (TRUE) {

		uint32_t ticksStart = SDL_GetTicks();
		
		state_ptr = (T_f)((*state_ptr)());
		
		SDL_Event e;
		// handle event on queue
		while (SDL_PollEvent(&e) != 0) {
			
			switch( e.type ){
				
				case SDL_QUIT:
					closeSDL();
					return 0;
				case SDL_KEYDOWN:
					printf("KEY PRESSED: %d\n",e.key.keysym.sym);
					keyboardStatus = keyboardStatus | keys[e.key.keysym.sym%sizeof(keys)];
					break;
                case SDL_KEYUP:
					printf("KEY RELEASED: %d\n",e.key.keysym.sym);
					keyboardStatus = keyboardStatus & ~keys[e.key.keysym.sym%sizeof(keys)];
                    break;
			}
		}

	
		/*{
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
		}	*/	
		
		drawFramebufferSDL();
		
		uint32_t delay = SDL_GetTicks()-ticksStart;
		if (delay>(1000/60-1)) delay = (1000/60-1);
		SDL_Delay(1000/60-delay);
	}    
}

