#pragma once
#include <msxhal.h>

////////////////////////////////////////////////////////////////////////
// Font transform functions, get a source shape, obtains a tansformation, 
// and returns its new size.

uint8_t Font_getBorderShape(const U8x8 source, U8x8 target);
