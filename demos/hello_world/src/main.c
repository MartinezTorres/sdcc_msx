// If DEBUG is defined, the function debugBorder changes the border color, otherwise it does nothing.
//#define DEBUG TRUE 

#include <msxhal.h>
#include <tms99X8.h>
#include <monospace.h>
#include <tile.h>
#include <rand.h>



#include <sdcc_msx/res/fonts/itfont-ek-zelda.h>

T_M2_MS_Font mainFont; // Big structures, like this one, its better to store them in global storage

// Here we create a monospace font (i.e., each character being 8x8 pixels). By initializing it as "Double", we use two tiles per character.
// By alternating between buffers we can obtain a larger combination of colors, as well as better color clash properties.
static void initFont() {

    static const U8x8 color0 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    static const U8x8 color1 = {BBlack+FDarkBlue, BBlack+FMagenta, BBlack+FMediumRed, BBlack+FLightRed, BBlack+FDarkYellow, BBlack+FLightBlue, BBlack+FDarkBlue, BBlack+FCyan};
    //static const U8x8 color0 = {BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed, BBlack+FLightRed};
    //static const U8x8 color1 = {BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow, BBlack+FLightYellow};
    M2_MS_setFontDouble(
	mainFont,
	MODE2_ALL_ROWS,
	font_zelda,
	M2_MS_transformBorder, // The color 1 will be applied to a "bold" version of the font.
	color0, color1); 
}


static void initSprites() {

    static const U8x8 sprites[] = { {
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

int main(void) {

    // Normal initialization routine
    msxhal_init(); // Bare minimum initialization of the msx support 
    Tiles_init(); // It initializes the global tile storage (i.e., to allow to load a font afterwards)
    TMS99X8_activateMode2(MODE2_ALL_ROWS); // Activates mode 2 and clears the screen (in black)
    
    initFont();

    // We also show a few sprites along the way :)
    initSprites();
    
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
	
	debugBorder(BMagenta);

	TMS99X8_activateBuffer(MODE2_BUFFER_0); 
	// We select buffer 0, we modify sprites on buffer 1
	debugBorder(BWhite);

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
	debugBorder(BTransparent);
	
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

