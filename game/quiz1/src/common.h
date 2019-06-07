#pragma once
#include <msxhal.h>

#include <tms99X8.h>
#include <common/res/fonts/itfont-ek-zelda.h>
#include <monospace.h>

extern T_SA SA0, SA1;
extern T_PN PN0, PN1;
//extern T_CT CT; // Pattern Color Table
//extern T_PG PG; // Pattern Generator Table
//extern T_SG SG; // Sprite Generator Table

extern T_M2_MS_Font mainFont;


#ifdef DEBUG
INLINE void debugTimming(uint8_t v) {
    TMS99X8_setRegister(7,v);
}
#else
INLINE void debugTimming() {}
#endif


