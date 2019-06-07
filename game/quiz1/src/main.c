//#define DEBUG

#include <msxhal.h>
#include <psg.h>

#include "common.h"
#include "typingGame.h"


// Mirror of sprite tables;
T_SA SA0, SA1;
T_PN PN0, PN1;
//T_CT CT; // Pattern Color Table
//T_PG PG; // Pattern Generator Table
//T_SG SG; // Sprite Generator Table

T_M2_MS_Font mainFont;


USING_PAGE_B(psg);
USING_PAGE_D(typingGame);

// Default AUDIO ISR

void audio_isr(); 
INLINE void audio_init() {
    
    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
    ayr_init();
    ayFX_init();
    PSG_init();
    restore_page_b(oldSegmentPageB);

    msxhal_install_isr(&audio_isr);
}

uint8_t frameNumber;
void audio_isr() { 
    
    uint8_t oldSegmentPageB;
    frameNumber++;
    return;
    debugTimming(0x55);
    oldSegmentPageB = load_page_b(SEGMENT_B(psg));

    debugTimming(0x88);
    ayr_spin();
    debugTimming(0x44);

    debugTimming(0x88);
    ayFX_spin();

    debugTimming(0x44);    
    PSG_syncRegisters();

    debugTimming(0x55);
    restore_page_b(oldSegmentPageB);    
    debugTimming(0x00);
}

INLINE void frontPage() {

    TMS99X8_activateMode2(MODE2_ALL_ROWS);
//    TMS99X8_activateMode2(MODE2_ROWS_0__7);
    
}


INLINE void fadeOffScreenAndAudio() {

    TMS99X8_activateMode2(MODE2_ALL_ROWS);
//    TMS99X8_activateMode2(MODE2_ROWS_0__7);
    Tiles_init();
    // DISABLE SCREEN
    {
    }
    // RESET AUDIO
    {
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));		
	ayr_init();
	restore_page_b(oldSegmentPageB);
    }
}


INLINE void start() {

    fadeOffScreenAndAudio();
    
// CLEAR MUSIC STATUS
    {
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));		
	ayr_init();
	restore_page_b(oldSegmentPageB);
    }

    // DRAW MENU
    {
	M2_MS_setFontDouble(
	    mainFont,
	    M2_MS_PAGES_123,
//	    M2_MS_PAGES_0,
	    &font_zelda[0],
	    M2_MS_transformBorder,
	    BBlack,FWhite,
	    BBlack,FLightGreen);
	    
	M2_MS_printAt(mainFont,2,2,"Guadalcanal '42");
	/*
	M2_MS_printAt(mainFont,(32-14)/2,16,"Careeer Mode");
	M2_MS_printAt(mainFont,(32-14)/2,17,"vs mode");
	M2_MS_printAt(mainFont,(32-14)/2,19,"Rank: ");
	
	M2_MS_printAt(mainFont,1,22,"Programmed by Manel Martinez");
	M2_MS_printAt(mainFont,3,23,"Olives i Conserves Torres");*/
    }
    
    // ENABLE SCREEN
    {
	    
    }
    
    // ENABLE AUDIO
    {
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));		
	//ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));
	restore_page_b(oldSegmentPageB);
    }
    
    // MAIN MENU
    //uint8_t selectedOption = 0;
   if(0) {		
	char msg[32];
	uint8_t msgl = 0;
	msg[0]=0;

	while (true) {
	    
	    uint8_t c = msxhal_getch();
	    if (c) {
		msg[msgl++]=c;
		msg[msgl] = 0;
		if (msgl>20) msgl = 0;
	    }
	    
	    
	    M2_MS_printAt(mainFont,2,2,msg);

	    
	    
	    TMS99X8_activateBuffer(MODE2_BUFFER_0);
	    HALT();
	    TMS99X8_activateBuffer(MODE2_BUFFER_1);
	    HALT();
	}
    }
    
   startTypingGame();
}

int main(void) {
    
    msxhal_init();

    load_page_b(SEGMENT_B(psg));
    load_page_d(SEGMENT_D(typingGame));
    
    audio_init();
    
    frontPage();

    for(;;) {
	start();
	HALT();
    }
}
