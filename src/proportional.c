#include <proportional.h>

////////////////////////////////////////////////////////////////////////
// PREPARE BORDER-FONT


void M2_PS_AddChar(
	M2_PS_Msg *propMsg, 
	const M2_PS_Font *font, 
	uint8_t c)
{
	uint8_t sz = font->sizes[c];
	const uint8_t *source = font->shapes[c];
	uint8_t *target = propMsg->shapePtr;
	uint8_t shift   = propMsg->skip & 7;
	{
		register uint8_t *rtarget = target;
		register uint8_t  rshift = shift;
		register const uint8_t *rsource = source;
		REPEAT8(
			*rtarget |= *rsource >> rshift;
			rtarget++;
			rsource++;
		)
	}
	if (sz+shift>8) {

		register uint8_t *rtarget = target+8;
		register uint8_t  rshift = 8-shift;
		register const uint8_t *rsource = source;
		REPEAT8(
			*rtarget = *rsource << rshift;
			rtarget++;
			rsource++;
		)
		propMsg->shapePtr += 8;
	}
	{
		propMsg->pos[propMsg->len++] = propMsg->skip;
		propMsg->skip += sz;
	}
}

void M2_PS_InitMsg(
	M2_PS_Msg *propMsg, 
	const M2_PS_Font *font, 
	const char *msg, uint8_t len, 
	uint8_t skip)
{
	propMsg->shapePtr = (uint8_t *)propMsg->shapes;
	propMsg->skip = skip;
	propMsg->len = 0;
	memset(propMsg->pos,0,sizeof(propMsg->pos));
	memset(propMsg->shapes,0,sizeof(propMsg->shapes));
	
	while (len--)
		M2_PS_AddChar(propMsg,font,*msg++ - 32);
}
