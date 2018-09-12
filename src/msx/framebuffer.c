#include <msx1hal.h>
#include <util.h>


inline void vdpSetWriteAddress(uint8_t high) { 
	
	VDP1 = 0; 
	NOP();
	VDP1 = 0x40 | high;
	NOP();	
}

inline void fillCharAsm() {
	__asm
		add a, a
		add a, a
		add a, a
		and a, d
		inc hl
		add a, (HL)
		add a, e
		out (_VDP0),a		
	__endasm;
}

inline void fillCharAsmPossible() {
	__asm
		rra
		rra
		rra
		and a, d
		inc hl
		add a, (HL)
		//add a, e
		out (_VDP0),a		
	__endasm;
}
		
		
volatile uint8_t pv_;
volatile uint8_t *p_;

uint8_t lovelyLoopIndex;
void fillFrameBuffer(uint8_t tiles[24][128], uint8_t PGaddress, uint16_t x, uint16_t y) {
	
	
	uint16_t displayMapPosY = y;
	uint16_t displayMapPosX = x;
	vdpSetWriteAddress(PGaddress);
	{
		uint16_t x2=(displayMapPosX+0x20)>>6;
		pv_ = ((x2&3)<<6);
		p_ = &tiles[19-y][(x2>>2)];
		
		lovelyLoopIndex = 20;

	__asm 
		push af
		push bc
		push de
		push hl
		push iy

		ld a,#0x3F
		ld d, a
		ld a,(_pv_)
		ld e, a
		ld hl, (_p_)
	__endasm;

	__asm 
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
		pop iy
		pop hl
		pop de
		pop bc
		pop af
	__endasm;

	}
}
