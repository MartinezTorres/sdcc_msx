#include <msxhal.h>
#include <tms99X8.h>
#include <monospace.h>
#include <tile.h>


#include <sdcc_msx/res/fonts/itfont-ek-zelda.h>

T_M2_MS_Font mainFont; // Big structures, like this one, its better to store them in global storage

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
    
    // We also show a few sprites along the way :)
    {
	const U8x8 sprites[] = { {
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000
	}, {
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00011000,
	    0b00011000,
	    0b00000000,
	    0b00000000,
	    0b00000000
	}, {
	    0b00000000,
	    0b00000000,
	    0b00011000,
	    0b00100100,
	    0b00100100,
	    0b00011000,
	    0b00000000,
	    0b00000000
	}, {
	    0b00000000,
	    0b00100100,
	    0b00000000,
	    0b10000001,
	    0b00000000,
	    0b10000001,
	    0b00000000,
	    0b00100100
	}};

	
	uint8_t i=0;
	for (i=0; i<32; i++) {
	    SA0[i].y = SA1[i].y = i*6;
	    SA0[i].x = SA1[i].x = rand16();
	    
	    SA0[i].pattern = (255-3*i)&0x1F;
	    SA0[i].color = BCyan;

	    SA1[i].pattern = (255-3*i)&0x1F;
	    SA1[i].color = BDarkRed;
	}

	for (i=0; i<20; i++)
	    TMS99X8_writeSprite8(i,sprites[0]);

	for (i=20; i<22; i++)
	    TMS99X8_writeSprite8(i,sprites[1]);

	for (i=22; i<25; i++)
	    TMS99X8_writeSprite8(i,sprites[2]);

	for (i=25; i<32; i++)
	    TMS99X8_writeSprite8(i,sprites[3]);

	TMS99X8_writeSpriteAttributes(MODE2_BUFFER_0,SA0); // We also use alternating buffers for the sprites.
	TMS99X8_writeSpriteAttributes(MODE2_BUFFER_1,SA1);
    }
	
	
    // Main loop, we alternate between buffers at each interruption.
    while (true) { 

	wait_frame();
	TMS99X8_activateBuffer(MODE2_BUFFER_0); 
	// We select buffer 0, we modify sprites on buffer 1
	TMS99X8_writeSpriteAttributes(MODE2_BUFFER_1,SA1);
	{
	    uint8_t i=0;
	    for (i=0; i<32; i++) {
		uint8_t y = SA1[i].y, p = SA1[i].pattern;
		SA1[i].y = (y==24*8?247:y+1);
		SA1[i].pattern = (p==32?0:p+1);
		if (p==1) SA0[i].x = SA1[i].x = rand16();
	    }
	}
	
	wait_frame();

	// We select buffer 1, so we modify sprites on buffer 0
	TMS99X8_activateBuffer(MODE2_BUFFER_1);
	{
	    uint8_t i=0;
	    for (i=0; i<32; i++) {
		SA0[i].y = SA1[i].y;
		SA0[i].x = SA1[i].x;
		SA0[i].pattern = SA1[i].pattern;
		
	    }
	}
	TMS99X8_writeSpriteAttributes(MODE2_BUFFER_0,SA0);
    };
    
    return 0;
}
