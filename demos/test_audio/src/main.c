// If DEBUG is defined, the function debugBorder changes the border color, otherwise it does nothing.
#define DEBUG TRUE 

#include <msxhal.h>
#include <tms99X8.h>
#include <monospace.h>
#include <tile.h>
#include <msx_string.h>



#include <sdcc_msx/res/fonts/itfont-ek-zelda.h>
T_M2_MS_Font mainFont;
static void initFont() {

    //static const U8x8 color0 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    //static const U8x8 color1 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    static const U8x8 color0 = {BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed};
    static const U8x8 color1 = {BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow};
    M2_MS_setFontDouble(
	mainFont,
	MODE2_ALL_ROWS,
	font_zelda,
	M2_MS_transformBorder, // The color 1 will be applied to a "bold" version of the font.
	color0, color1); 
}


/*

#include <res/midi/indy4/theme_and_opening_credits.mid.h>
#include <res/midi/chopin/chpn_op10_e05.mid.h>
#include <res/midi/chopin/chpn_op10_e01.mid.h>
#include <res/midi/chopin/chpn_op10_e12.mid.h>
#include <res/midi/indy4/theme_and_opening_credits.mid.h>


USING_PAGE_B(psg);
USING_PAGE_C(chpn_op10_e05_mid);
USING_PAGE_C(chpn_op10_e01_mid);
USING_PAGE_C(chpn_op10_e12_mid);
USING_PAGE_C(theme_and_opening_credits_mid);
USING_PAGE_C(inicio_juego_afb);

void audio_isr();

void audio_isr() {
    
    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));

    TMS99X8_setRegister(7,0x77);
    ayr_spin();
    TMS99X8_setRegister(7,0x44);

    TMS99X8_setRegister(7,0x88);
    ayFX_spin();
    TMS99X8_setRegister(7,0x44);
    
    PSG_syncRegisters();
    TMS99X8_setRegister(7,0x55);

    restore_page_b(oldSegmentPageB);    
    TMS99X8_setRegister(7,0);    
}

INLINE int start() {

    bool released = true;
   
   // TMS99X8_activateMode2(false);
    
    while (true) {
	
	uint8_t key = msxhal_joystick_read(0);
	
	if (key!=0 && released) {
	    
	    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
	    DI();

	    //if (key==J_SPACE) ayr_play(&chpn_op10_e05_mid,SEGMENT_C(chpn_op10_e05_mid));
	    //if (key==J_DOWN)  ayr_play(&chpn_op10_e01_mid,SEGMENT_C(chpn_op10_e01_mid));
	    //if (key==J_UP)    ayr_play(&chpn_op10_e12_mid,SEGMENT_C(chpn_op10_e12_mid));
	    //if (key==J_LEFT)  ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));

	    ayFX_afb(inicio_juego_afb,SEGMENT_C(inicio_juego_afb),key>>4,15,0);
	    EI();
	    restore_page_b(oldSegmentPageB);    
	}
	
	released = (key==0);
	HALT();
    };

}

int main(void) {

    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));

    msxhal_init();
    ayr_init();
    ayFX_init();
    msxhal_install_isr(&audio_isr);
    
    //PSG_init();

    restore_page_b(oldSegmentPageB);    

    start();
}
*/


#include <psg.h>
USING_PAGE_B(psg);

typedef struct {
    
    const char *name;
    const uint8_t segment;
    const void *item;
} AudioMenuItem;

////////////////////////////////////////////////////////////////////////
// Load AFB sources

#include <res/ayfx/inicio_juego.afb.h>

USING_PAGE_C(inicio_juego_afb);

static const AudioMenuItem afbItems[] = {
    { "demo afb 1", SEGMENT_TO_PAGE_C(inicio_juego_afb), inicio_juego_afb },
    { "demo afb 2", SEGMENT_TO_PAGE_C(inicio_juego_afb), inicio_juego_afb },
};

////////////////////////////////////////////////////////////////////////
// Load MIDI sources

#include <res/midi/indy4/theme_and_opening_credits.mid.h>
USING_PAGE_C(theme_and_opening_credits_mid);

#include <res/midi/chopin/chpn_op10_e05.mid.h>
USING_PAGE_C(chpn_op10_e05_mid);

#include <res/midi/chopin/chpn_op10_e01.mid.h>
USING_PAGE_C(chpn_op10_e01_mid);

#include <res/midi/chopin/chpn_op10_e12.mid.h>
USING_PAGE_C(chpn_op10_e12_mid);

static const AudioMenuItem ayrItems[] = {
    { "Indy4: theme_and_opening_credits", SEGMENT_TO_PAGE_C(theme_and_opening_credits_mid), &theme_and_opening_credits_mid },
    { "Chopin: OP10_e05", SEGMENT_TO_PAGE_C(chpn_op10_e05_mid), &chpn_op10_e05_mid },
    { "Chopin: OP10_e01", SEGMENT_TO_PAGE_C(chpn_op10_e01_mid), &chpn_op10_e01_mid },
    { "Chopin: OP10_e12", SEGMENT_TO_PAGE_C(chpn_op10_e12_mid), &chpn_op10_e12_mid },
};


////////////////////////////////////////////////////////////////////////
// AUDIO ISR

void audio_isr(void);

void audio_isr(void) {
    
    uint8_t oldSegmentPageB = load_page_b(SEGMENT_TO_PAGE_B(psg));

    TMS99X8_setRegister(7,BWhite);
    PSG_init();    

    TMS99X8_setRegister(7,BLightRed);
    ayr_spin();

    TMS99X8_setRegister(7,BLightGreen);
    ayFX_spin();

    TMS99X8_setRegister(7,BLightBlue);    
    PSG_syncRegisters();
    
    TMS99X8_setRegister(7,BWhite);
    restore_page_b(oldSegmentPageB);    

    TMS99X8_setRegister(7,BBlack);    
}


////////////////////////////////////////////////////////////////////////
// MAIN

char msg[20];
uint8_t afbNumItems;

int main(void) {
    
    // Normal initialization routine
    msxhal_init(); // Bare minimum initialization of the msx support 
    Tiles_init(); // It initializes the global tile storage (i.e., to allow to load a font afterwards)
    TMS99X8_activateMode2(MODE2_ALL_ROWS); // Activates mode 2 and clears the screen (in black)
    
    load_page_b(SEGMENT_TO_PAGE_B(psg));
    
    ayr_init();
    ayFX_init();
    msxhal_install_isr(&audio_isr);

    initFont();
   
    M2_MS_printAt(mainFont, 0, 0, "SDCC_MSX audio test");

    M2_MS_printAt(mainFont, 2, 2, "PSG arb Player: 001/");
    {
	uint8_t i=0;
	afbNumItems = 0;
	for (i=0; i<sizeof(afbItems)/sizeof(AudioMenuItem); i++)
	    afbNumItems += ayFX_afb_getNSounds(afbItems[i].item, afbItems[i].segment);
	    
	M2_MS_puts(mainFont, uint16ToString(msg,afbNumItems,3));
    }
    M2_MS_printAt(mainFont, 0, 4,afbItems[0].name);
    M2_MS_puts(mainFont, " #001");


    M2_MS_printAt(mainFont, 2, 8, "PSG MIDI Player: 001/");
    M2_MS_puts(mainFont, uint16ToString(msg,sizeof(ayrItems)/sizeof(AudioMenuItem),3));
    M2_MS_printAt(mainFont, 0, 10,ayrItems[0].name);
    
    
    // Main loop, we alternate between buffers at each interruption.
    {
	uint8_t classSelection = 0;
	uint8_t afbIndex = 0;
	uint8_t afbSelection = 0;
	uint8_t afbSubSelection = 0;
	uint8_t ayrSelection = 0;
	uint8_t isPressed = 0;
	uint8_t key = 0;

	while (true) { 

	    wait_frame();
	    TMS99X8_swapBuffers(); 
	    
	    key = msxhal_joystick_read(0);
	    
	    if (isPressed==0 && key!=0) {
		if        (key==J_UP) {
		    classSelection--;
		    if (classSelection==255) classSelection=0;

		} else if (key==J_DOWN) {
		    classSelection++;
		    if (classSelection==2) classSelection=1;

		} else if (key==J_LEFT) {
		    if (classSelection==0) {
			if (afbSubSelection) {
			    afbSubSelection--;
			    afbIndex--;
			} else if (afbSelection) {
			    afbSelection--;
			    afbSubSelection = ayFX_afb_getNSounds(afbItems[afbSelection].item, afbItems[afbSelection].segment)-1;
			    afbIndex--;
			}
		    }
		    if (classSelection==1 && ayrSelection) ayrSelection--; 

		} else if (key==J_RIGHT) {
		    if (classSelection==0) {
			
			if (afbSubSelection+1 < ayFX_afb_getNSounds(afbItems[afbSelection].item, afbItems[afbSelection].segment)) { 
			    afbSubSelection++;
			    afbIndex++;
			    
			} else if (afbSubSelection+1 == ayFX_afb_getNSounds(afbItems[afbSelection].item, afbItems[afbSelection].segment) 
			    && afbSelection+1<(sizeof(afbItems)/sizeof(AudioMenuItem))) {
			    afbSelection++;
			    afbSubSelection = 0;
			    afbIndex++;
			}
		    }
		    if (classSelection==1 && ayrSelection+1<(sizeof(ayrItems)/sizeof(AudioMenuItem))) {
			ayrSelection++;
		    } 
		} else if (key==J_SPACE) {
		    if (classSelection==0) {
			ayFX_afb(afbItems[afbSelection].item, afbItems[afbSelection].segment,afbSubSelection,15,0);
		    }
		    if (classSelection==1) {
			ayr_play(ayrItems[ayrSelection].item, ayrItems[ayrSelection].segment);
		    }
		    
		}

		M2_MS_printAt(mainFont,0, 4,afbItems[afbSelection].name);
		M2_MS_puts(mainFont, " #");
		M2_MS_puts(mainFont, uint16ToString(msg,afbSubSelection+1,3));
		M2_MS_puts(mainFont, "                                ");
		
		M2_MS_printAt(mainFont,0,10,ayrItems[ayrSelection].name);
		M2_MS_puts(mainFont, "                                ");

		M2_MS_printAt(mainFont,18, 2, uint16ToString(msg,afbIndex+1,3));
		M2_MS_printAt(mainFont,19, 8, uint16ToString(msg,ayrSelection+1,3));

		isPressed=1;
	    } else if (key==0) {
		isPressed=0;
	    }
	    
	    if (classSelection == 0) {
		M2_MS_printAt(mainFont,1,2,">");
		M2_MS_printAt(mainFont,1,8," ");
	    } else {
		M2_MS_printAt(mainFont,1,2," ");
		M2_MS_printAt(mainFont,1,8,">");		
	    }
	};
    }
    
    return 0;
}
