#include <font.h>

uint8_t Font_getBorderShape(const U8x8 source, U8x8 target) {
	
	uint8_t i, begin, end;
	memset(target,0,sizeof(target));

	{
		uint8_t mask = 0;
		for (i=0; i<8; i++)
			mask |= source[i];
		
		if (mask==0) mask = 3;
		
		begin = 1;
		{ 
			uint8_t m = mask<<1;
			if (m==0) m = 128;
			while ((m&128)==0) {
				m = m<<1;
				begin++;
			}
		}
		end = 8;
		{ 
			uint8_t m = mask;
			while ((m&1)==0) {
				m = m>>1;
				end--;
			}
		}
	}

	
	for (i=0; i<7; i++) {
		target[i] |= source[i+1];
		target[i+1] |= source[i];
	}

	for (i=0; i<8; i++) {
		target[i] = target[i] | (target[i]<<1) | (target[i]>>1);
		target[i] &= ~source[i];
		target[i] = target[i] << (begin-1);
	}
	
	return end-begin;
}

