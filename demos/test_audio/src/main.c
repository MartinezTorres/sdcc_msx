// If DEBUG is defined, the function debugBorder changes the border color, otherwise it does nothing.
#define DEBUG TRUE 

#include <msxhal.h>
#include <tms99X8.h>
#include <monospace.h>
#include <tile.h>
#include <psg.h>



#include <sdcc_msx/res/fonts/itfont-ek-zelda.h>

T_M2_MS_Font mainFont; // Big structures, like this one, its better to store them in global storage

// Here we create a monospace font (i.e., each character being 8x8 pixels). By initializing it as "Double", we use two tiles per character.
// By alternating between buffers we can obtain a larger combination of colors, as well as better color clash properties.
void initFont() {

    //const U8x8 color0 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    //const U8x8 color1 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    const U8x8 color0 = {BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed};
    const U8x8 color1 = {BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow};
    M2_MS_setFontDouble(
	mainFont,
	MODE2_ALL_ROWS,
	font_zelda,
	M2_MS_transformBorder, // The color 1 will be applied to a "bold" version of the font.
	color0, color1); 
}


/*#include <res/ayfx/inicio_juego.afb.h>

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


int main(void) {

    // Normal initialization routine
    msxhal_init(); // Bare minimum initialization of the msx support 
    Tiles_init(); // It initializes the global tile storage (i.e., to allow to load a font afterwards)
    TMS99X8_activateMode2(MODE2_ALL_ROWS); // Activates mode 2 and clears the screen (in black)
    
    initFont();
   
    // Set the hello world, also getting some information from the BIOS (i.e., the frequency of this MSX).
    if (msxhal_get_interrupt_frequency()==MSX_FREQUENCY_60HZ) {
	M2_MS_printAt(mainFont, 5, 5, "This is a 60Hz MSX!");
	M2_MS_printAt(mainFont, 8, 7, "Kon'nichiwa sekai!");
    } else {
	M2_MS_printAt(mainFont, 0, 5, "This is a 50Hz MSX! Hello World!");
    }
    
    // Main loop, we alternate between buffers at each interruption.
    while (true) { 

	wait_frame();
	TMS99X8_swapBuffers(); 
    };
    
    return 0;
}
