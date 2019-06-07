//#define DEBUG


////////////////////////////////////////////////////////////////////////
// Portada (opcional)
//
// Menu principal
// - Opciones: 1 player, 2 players, entrar codigo para rango
//
// Si 1 player: escoger misión (icono grande)
// Si 2 player: escoger modo (icono grande)
//
// Al entrar a misión: 
// Briefing (con pausa al final)
// Mainn


#include <msxhal.h>
#include <tms99X8.h>
#include <psg.h>
#include <monospace.h>

#include "guadalcanal.h"

#include <common/res/fonts/itfont-ek-zelda.h>

#include <res/ayfx/inicio_juego.afb.h>

#include <res/midi/indy4/theme_and_opening_credits.mid.h>
#include <res/midi/chopin/chpn_op10_e05.mid.h>
#include <res/midi/chopin/chpn_op10_e01.mid.h>
#include <res/midi/chopin/chpn_op10_e12.mid.h>

USING_PAGE_B(psg);
USING_PAGE_C(chpn_op10_e05_mid);
USING_PAGE_C(chpn_op10_e01_mid);
USING_PAGE_C(chpn_op10_e12_mid);
USING_PAGE_C(theme_and_opening_credits_mid);
USING_PAGE_C(inicio_juego_afb);

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

/*
INLINE void loadFont();
INLINE void loadFont() {
    
    U8x8 color;
    color[0] = BTransparent + FWhite;
    color[1] = BTransparent + FWhite;
    color[2] = BTransparent + FWhite;
    color[3] = BTransparent + FWhite;
    color[4] = BTransparent + FWhite;
    color[5] = BTransparent + FWhite;
    color[6] = BTransparent + FWhite;
    color[7] = BTransparent + FWhite;

    M2_MS_loadFont(MODE2_PGPAGE_0, font_zelda, color,' ','z');
    
}


INLINE void start() {

    bool released = true;
    TMS99X8_activateMode2(MODE2_ENABLE_PGPAGE_0);
    
    while (true) {
	
	uint8_t key = msxhal_joystick_read(0);
	
	if (key!=0 && released) {
	    
	    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
	    DI();

	    if (key==J_SPACE) ayr_play(&chpn_op10_e05_mid,SEGMENT_C(chpn_op10_e05_mid));
	    if (key==J_DOWN)  ayr_play(&chpn_op10_e01_mid,SEGMENT_C(chpn_op10_e01_mid));
	    if (key==J_UP)    ayr_play(&chpn_op10_e12_mid,SEGMENT_C(chpn_op10_e12_mid));
	    if (key==J_LEFT)  ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));

	    ayFX_afb(inicio_juego_afb,SEGMENT_C(inicio_juego_afb),key>>4,15,0);
	    EI();
	    restore_page_b(oldSegmentPageB);    
	}
	
	released = (key==0);
	HALT();
    };

}*/

// Mirror of sprite tables;
T_SA SA0, SA1;
INLINE void startFrontPage() {

    TMS99X8_activateMode2(MODE2_ROWS_0__7);
    
    if (0) {
	    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));		
		ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));
	    restore_page_b(oldSegmentPageB);
	}
//    while (msxhal_joystick_read(0)==0) HALT();
}


INLINE void fadeOffScreenAndAudio() {

    TMS99X8_activateMode2(MODE2_ROWS_0__7);
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

T_M2_MS_Font mainFont;

INLINE void startMenu() {

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
	    &font_zelda[32],
	    M2_MS_transformBorder,
	    BBlack,FWhite,
	    BBlack,FLightGreen);
	    
	M2_MS_printAt(mainFont,2,2,"Guadalcanal '42");
	
	M2_MS_printAt(mainFont,(32-14)/2,16,"Careeer Mode");
	M2_MS_printAt(mainFont,(32-14)/2,17,"vs mode");
	M2_MS_printAt(mainFont,(32-14)/2,19,"Rank: ");
	
	M2_MS_printAt(mainFont,1,22,"Programmed by Manel Martinez");
	M2_MS_printAt(mainFont,3,23,"Olives i Conserves Torres");
    }
    
    // ENABLE SCREEN
    {
	    
    }
    
    // ENABLE AUDIO
    {
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));		
	ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));
	restore_page_b(oldSegmentPageB);
    }
    
    // MAIN MENU
    //uint8_t selectedOption = 0;
    {		
	while (true) {
	    
	    
	    
	    TMS99X8_activateBuffer(MODE2_BUFFER_0);
	    HALT();
	    TMS99X8_activateBuffer(MODE2_BUFFER_1);
	    HALT();
	}
    }
}



INLINE void startLevel(uint8_t levelNum) {

    // CLEAR MUSIC STATUS
    fadeOffScreenAndAudio();
    
    //TMS99X8_disable();
    
    // DRAW MAIN GAME SCREEN
    {
	M2_MS_setFontDouble(
	    mainFont,
	    M2_MS_PAGES_123,
	    &font_zelda[32],
	    M2_MS_transformBorder,
	    BBlack,FWhite,
	    BBlack,FLightGreen);
	    
	M2_MS_printAt(mainFont,2,2,"Guadalcanal '42");
	
	M2_MS_printAt(mainFont,(32-14)/2,16,"Careeer Mode");
	M2_MS_printAt(mainFont,(32-14)/2,17,"vs mode");
	M2_MS_printAt(mainFont,(32-14)/2,19,"Rank: ");
	
	M2_MS_printAt(mainFont,1,22,"Programmed by Manel Martinez");
	M2_MS_printAt(mainFont,3,23,"Olives i Conserves Torres");
    }
    
    // ENABLE SCREEN
    //TMS99X8_enable();
    
    // ENABLE AUDIO
    //
    
    // MAIN MENU
    //uint8_t selectedOption = 0;
    {		
	while (true) {
	    
	    
	    
	    TMS99X8_activateBuffer(MODE2_BUFFER_0);
	    HALT();
	    TMS99X8_activateBuffer(MODE2_BUFFER_1);
	    HALT();
	}
    }
}


int main(void) {

    msxhal_init();
    audio_init();
    
    startFrontPage();
    
    while (true) {
	startMenu();
	HALT();
    }
}


