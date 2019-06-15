#pragma once
#include <msxhal.h>
	
////////////////////////////////////////////////////////////////////////
// Generic String API
//
// Text API where Tiles are generated for each message.
//

//int sprintf(char *s, const char *format, ...);
char *uint16ToString(char *msg, uint16_t v, uint8_t zeroPad);
