#pragma once
#include <msxhal.h>

////////////////////////////////////////////////////////////////////////
// REWRITE OF THE API IN PROGRESS

/*

typedef struct {
	uint8_t sizes[96];	
	U8x8 shapes[96]; // Character shapes 
} M2_PS_Font;

typedef struct {
	uint8_t *shapePtr;
	uint8_t skip,len;
	uint8_t pos[64];
	U8x8 shapes[32]; // Character shapes 
} M2_PS_Msg;

INLINE void M2_PS_prepareFont(
	M2_PS_Font *font, 
	const U8x8 monospacedFont[96],
	uint8_t (*transform)(const U8x8, U8x8)
)
{
	uint8_t i;
	for (i=0; i<96; i++)
		font->sizes[i] = (*transform)(monospacedFont[i], font->shapes[i]);
}

bool M2_PS_AddChar(
	T_M2_MS_Font font,
	char c
	);

void M2_PS_InitMsg(
	M2_PS_Msg *propMsg, 
	const M2_PS_Font *font,
	const char *msg, uint8_t len, 
	uint8_t skip);
*/
