
#define DEBUG

#include "common.h"
#include "typingGame.h"

#include "dict/american-english.h"
#define WORDS words_american_english

#include <rand.h>
#include <msx_string.h>


////////////////////////////////////////////////////////////////////////
// PREPARE 1PX FONT




////////////////////////////////////////////////////////////////////////
// PREPARE SCREEN
typedef struct {	
	U8x8 backgroundShape[8][4][2];
	U8x8 backgroundColor[8][4][2];
} T_PlayscreenStatus;

static T_PlayscreenStatus screen0, screen1;

static void areYouReady() {}

static void countDown() {}

static void prepareScreen() {
	
	// We use mirrowed tiles.
	
	
	
	
	
	
	//E.g.
	
	// 0x00, 0x01, 0x20, 0x21, 0x80 ... // B&W characters: 16*4 = 64.... not much.
	// 0x02, 0x03, 0x22, 0x23,
	// ..
	// 0x0E, 0x0F, 0x2E, 0x2F,
	
	TMS99X8_activateMode2(MODE2_ROWS_0__7| MODE2_ROWS16_23);
	Tiles_freeAll();
	//

// 64 tiles reserved for the background.
// Iff we assume a máximum of 8 simultaneous words, with a maximum word size of 12, 
	uint8_t i,j,k;
	for (i=0; i<64; i++) 
		Tiles_alloc(0,i);
	for (i=0; i<8; i++) {
		for (j=0; j<32; j++) {
			PN0[0]
			TMS99X8_memset(MODE2_ADDRESS_PN0 + (((uint16_t)i)<<5) + j,       (i<<4) + (j&3), 1);
			TMS99X8_memset(MODE2_ADDRESS_PN1 + (((uint16_t)i)<<5) + j, 128 + (i<<4) + (j&3), 1);
		}
	}
	//memset()


	TMS99X8_memcpy(MODE2_ADDRESS_PG, (const void *)backgroundShape, sizeof(backgroundShape));
	TMS99X8_memcpy(MODE2_ADDRESS_CT, (const void *)backgroundColor, sizeof(backgroundColor));
}




////////////////////////////////////////////////////////////////////////
// CALCULATE WORDS


static const char *allWords[8][64];
static uint8_t allWordsLength[8][64];

static void initWordsDifficulty( const char *src, const char **dst,  uint8_t *len) {
	uint8_t nWord = 64;
	while (nWord--) {
		uint8_t l = 0;
		*dst++ = src;
		while (*src++ != ' ')
			l++;
			
		*len++ = l;
	}
}

static void initWords(const char *words[8][64], uint8_t wordsLength[8][64]) {
	uint8_t i;
	for (i=0; i<8; i++)
		initWordsDifficulty(WORDS[i], words[i], wordsLength[i]);
}

typedef struct {
const char *words[128];
uint8_t wordLengths[128];
uint8_t wordDifficulties[128];
uint8_t nWords;
} CurrentLevelInfo;

CurrentLevelInfo levelInfo;
	
static void initLevelWords(const uint8_t * const levelDescription) {
	
	uint8_t ld[8];
	uint8_t i;
	for (i=0; i<8; i++) 
		ld[i] = levelDescription[i];

	levelInfo.nWords=0;
	while (true) {
		
		uint8_t finished = true;
		for (i=0; i<8; i++) {
			
			uint8_t wordIdx = rand16()&63;
			if (ld[i]==0) continue;
			finished = false;
			if (rand16() & 0x07) continue;
			
			levelInfo.words[levelInfo.nWords] = allWords[i][wordIdx];
			levelInfo.wordLengths[levelInfo.nWords] = allWordsLength[i][wordIdx];
			levelInfo.wordDifficulties[levelInfo.nWords] = i;
			
			ld[i]--;
			levelInfo.nWords++;
		}
		if (finished) break;
	}
}



////////////////////////////////////////////////////////////////////////
// RAINBOW BACKGROUND

static uint8_t mask1;
static uint8_t mask2;
static uint8_t count1;
static uint8_t count2;

static void initRainbowBackground() {
	mask1 = 0; mask2 = 0; count1 = 0; count2 = 0;
}

static void updateRainbowBackground(uint8_t bufferIdx) {

	
	if (bufferIdx == 0) {
		debugTimming(5);
		//TMS99X8_memcpy(MODE2_ADDRESS_PG+16*8, (const void *)backgroundShape[1], sizeof(backgroundShape[1]));
		TMS99X8_memcpy(MODE2_ADDRESS_CT+16*8, (const void *)backgroundColor[1], sizeof(backgroundColor[1]));
		debugTimming(2);
		
		if (count1--==0) {
			count1 = 4;
			if (mask1==0) mask1=8;
			mask1--;
			memset((void *)backgroundShape[1], (0xF0>>mask1)+(0x0F<<(8-mask1)),16*8);
		}
		
		debugTimming(0);
	} else {
		
		debugTimming(5);
		//TMS99X8_memcpy(MODE2_ADDRESS_PG, (const void *)backgroundShape[0], sizeof(backgroundShape[0]));
		TMS99X8_memcpy(MODE2_ADDRESS_CT, (const void *)backgroundColor[0], sizeof(backgroundColor[0]));
		debugTimming(3);


		if (count2--==0) {
			count2 = 3;
			if (mask2==0) mask2=9;
			mask2--;
			mask2=0;
			memset((void *)backgroundShape[0], (0x00>>mask2),16*8);
		}
		debugTimming(0);
	}			
}


////////////////////////////////////////////////////////////////////////
// MAIN LEVEL LOOP

static void startLevel(const uint8_t * const levelDescription, void (*updateBackground)(uint8_t)) {
	
	initLevelWords(levelDescription);

	// Are you Ready?
	areYouReady();
	
	// Count Down
	countDown();
	
	prepareScreen();

	// Main routine
	{
		uint8_t bufferIdx = 0;
		for (;;) {
			debugTimming(15);
			{
				uint8_t c = getchar();
				
			}
			
			updateBackground(bufferIdx);
			TMS99X8_activateBuffer(bufferIdx?MODE2_BUFFER_1:MODE2_BUFFER_0);
			bufferIdx = ! bufferIdx;
			HALT();
		}
	}
}

static const uint8_t levelDescriptions[][8] = {
	{2,2,2,2,2,0,0,0},
};




void startTypingGame() {
	
	enable_keyboard_routine = true;

	
//	prepareBorderFont(&pfZelda, &font_zelda[0]);

	initWords(allWords, allWordsLength);
	rand16_seedTime();
	
	initRainbowBackground();
	startLevel(levelDescriptions[0], updateRainbowBackground);
	
}



