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

struct {
	
	uint8_t ram[0x4000];
	
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
} TMS9918Status;



static inline void drawMode2(const T_PN PN, const T_CT CT, const T_PG PG, const T_SA SA, const T_SG SG) {
	
//	printf("PN! %X\n", &PN[0][0]-&TMS9918Status.ram[0]);
//	printf("CT! %X\n", &CT[0][0][0]-&TMS9918Status.ram[0]);
//	printf("PG! %X\n", &PG[0][0][0]-&TMS9918Status.ram[0]);
//	printf("SA! %X\n", (uint8_t *)&SA[0]-&TMS9918Status.ram[0]);
//	printf("SG! %X\n", &SG[0][0]-&TMS9918Status.ram[0]);

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
}

static inline void drawScreen() {

	for (int i=0; i<TEX_HEIGHT; i++)
		for (int j=0; j<TEX_WIDTH; j++)
			framebuffer[i][j] = colors[TMS9918Status.backdrop];
			
	if (TMS9918Status.blankScreen) return;

	if (TMS9918Status.mode2) {
		const T_PN *PN = (T_PN *)&TMS9918Status.ram[(uint16_t)(TMS9918Status.pn10)<<10];
		const T_CT *CT = (T_CT *)&TMS9918Status.ram[(uint16_t)(TMS9918Status.ct6&0x80)<< 6];
		const T_PG *PG = (T_PG *)&TMS9918Status.ram[(uint16_t)(TMS9918Status.pg11&0xFC)<<11];
		const T_SA *SA = (T_SA *)&TMS9918Status.ram[(uint16_t)(TMS9918Status.sa7 )<< 7];
		const T_SG *SG = (T_SG *)&TMS9918Status.ram[(uint16_t)(TMS9918Status.sg11)<<11];

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

void setTMS9918_setMode2() {
	
	memset(&TMS9918Status,0,sizeof(TMS9918Status));
	TMS9918Status.mode2 = 1;
	TMS9918Status.generateInterrupts = 1;
	TMS9918Status.mem416K = 1;
	
	TMS9918Status.pn10 =  ADDRESS_PN0 >> 10;
	TMS9918Status.ct6  = (ADDRESS_CT  >>  6) | 0b01111111;
	TMS9918Status.pg11 = (ADDRESS_PG  >> 11) | 0b00000011;
	TMS9918Status.sa7  =  ADDRESS_SA0 >>  7;
	TMS9918Status.sg11 =  ADDRESS_SG >> 11;
}

void setTMS9918_activatePage0() {

	TMS9918Status.pn10 = ADDRESS_PN0 >> 10;
	TMS9918Status.sa7  = ADDRESS_SA0 >>  7;
}
void setTMS9918_activatePage1() {
	
	TMS9918Status.pn10 = ADDRESS_PN1 >> 10;
	TMS9918Status.sa7  = ADDRESS_SA1 >>  7;
}

void setTMS9918_setRegister(uint8_t reg, uint8_t val) {
	
	TMS9918Status.reg[reg] = val;
}

void setTMS9918_write(uint16_t dst, uint8_t *src, uint16_t sz) {
	
//	printf("dst: %04X, sz: %04X\n",dst, sz);
//	for (int i=0; i<10; i++) printf("%02X:",src[i]); printf("\n");
//	for (int i=0; i<10; i++) printf("%02X:",TMS9918Status.ram[dst+i]); printf("\n");
	memcpy(&TMS9918Status.ram[dst], src, sz);
//	for (int i=0; i<10; i++) printf("%02X:",TMS9918Status.ram[dst+i]); printf("\n");
}


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


// SDL BACKEND
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* tex = NULL;

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


int main() {
	
	
//	for (int i=0; i<8; i++) printf("%d: %02X\n",i,TMS9918Status.reg[i]);		
//	TMS9918Status.mode2 = 1;
//	TMS9918Status.backdrop = 0xA;
//	for (int i=0; i<8; i++) printf("%d: %02X\n",i,TMS9918Status.reg[i]);
	
	
	T_f	state_ptr = (T_f)start;
	
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
		
		drawScreen();
		
		displayFramebufferSDL();
		
		uint32_t delay = SDL_GetTicks()-ticksStart;
		if (delay>(1000/60-1)) delay = (1000/60-1);
		SDL_Delay(1000/60-delay);
	}    
}

