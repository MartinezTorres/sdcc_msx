#include <msx1hal.h>
#include <util.h>

__sfr __at 0x98 VDP0;
__sfr __at 0x99 VDP1;


inline void NOP(void) { __asm nop __endasm; }
inline void DI(void) { __asm di __endasm; }
inline void EI(void) { __asm ei __endasm; }


inline void vdpSetWriteAddress(uint8_t high) { 
	
//	DI();
	VDP1 = 0;
	NOP();
	VDP1 = high+0b0100000;
//	EI();
}

inline void fillCharAsm() {
	__asm
		add a, a
		add a, a
		and a, d
		inc hl
		add a, (HL)
		add a, e
		out (_VDP0),a		
	__endasm;
}
		
		
volatile uint8_t pv_;
volatile uint8_t *p_;

uint8_t lovelyLoopIndex;
void fillFrameBuffer(uint8_t tiles[24][128], uint16_t PGaddress, uint16_t x, uint16_t y) {
	
	
	uint16_t displayMapPosY = y;
	uint16_t displayMapPosX = x;
	vdpSetWriteAddress(PGaddress);
	{
		uint16_t x2=(displayMapPosX+0x20)>>6;
		pv_ = 0x80 + ((x2&3)<<4);
		p_ = &tiles[19-y][(x2>>2)];
		
		lovelyLoopIndex = 20;

	__asm 
//		push bc
//		push de
//		push hl

		ld a,#0x0F
		ld d, a
		ld a,(_pv_)
		ld e, a
		ld hl, (_p_)
	__endasm;

		//for (i=0; i<20; i++) {
	__asm 
//		push bc
//		push de
//		push hl

lli1$:	ld a,(hl)
	__endasm;
	

	{
		REPEAT32( fillCharAsm(); );
	}

	__asm
		ld bc,#128+#32
		or a
		sbc hl,bc
		ld	iy,#_lovelyLoopIndex
		dec (iy)
		jp NZ,lli1$
//		pop hl
//		pop de
//		pop bc
	__endasm;

		//}	
	}
}
