// If DEBUG is defined, the function debugBorder changes the border color, otherwise it does nothing.
#define DEBUG TRUE 

#include <msxhal.h>
#include <tms99X8.h>

#include <monospace.h>
USING_PAGE_C(monospace);

#include <tile.h>
#include <msx_string.h>



#include <res/fonts/font1.png.h>
USING_PAGE_D(font1_png);

T_M2_MS_Font mainFont;
static void initFont() {

    uint8_t oldPageD = load_page_d(SEGMENT_TO_PAGE_D(font1_png));

    //static const U8x8 color0 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    //static const U8x8 color1 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    static const U8x8 color0 = {BBlack+FWhite, BBlack+FWhite, BBlack+FWhite, BBlack+FWhite, BBlack+FWhite, BBlack+FWhite, BBlack+FWhite, BBlack+FWhite};
    //static const U8x8 color1 = {BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow};
    M2_MS_setFontSimple(
	mainFont,
	MODE2_ALL_ROWS,
	font1,
	//M2_MS_transformBorder, // The color 1 will be applied to a "bold" version of the font.
	color0); 
	
    restore_page_d(oldPageD);
}

#include <psg.h>
USING_PAGE_B(psg);

typedef struct {
    
    const char *name;
    const uint8_t segment;
    const void *item;
} AudioMenuItem;

////////////////////////////////////////////////////////////////////////
// Load AFB sources

#include <res/ayfx/test.afb.h>
USING_PAGE_C(test_afb);

#include <res/ayfx/streetsofrage_2.afb.h>
USING_PAGE_C(streetsofrage_2_afb);

#include <res/ayfx/zedragon.afb.h>
USING_PAGE_C(zedragon_afb);

static const AudioMenuItem afbItems[] = {
    { "zedragon", SEGMENT_TO_PAGE_C(zedragon_afb), zedragon_afb },
    { "streetsofrage2", SEGMENT_TO_PAGE_C(streetsofrage_2_afb), streetsofrage_2_afb },
    { "test", SEGMENT_TO_PAGE_C(test_afb), test_afb },
};

////////////////////////////////////////////////////////////////////////
// Load MIDI sources
//
// the list of midi sources can be automatically updated by running:
//    make updateMidiHeader
// this will add al files under /res/midi into the following header:
#include <midiSources.h>

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
    load_page_c(SEGMENT_TO_PAGE_C(monospace));
    
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
			    && afbSelection+1<(uint8_t)(sizeof(afbItems)/sizeof(AudioMenuItem))) {
			    afbSelection++;
			    afbSubSelection = 0;
			    afbIndex++;
			}
		    }
		    if (classSelection==1 && ayrSelection+1<(uint8_t)(sizeof(ayrItems)/sizeof(AudioMenuItem))) {
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
