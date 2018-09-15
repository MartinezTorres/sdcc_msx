
#define TRUE 1
#define FALSE 0

static inline uint8_t reverse8(uint8_t i) {
   
   register uint8_t b = i;
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

static inline uint16_t reverse16(uint16_t i) {
   
   register uint16_t b = i;
   b = (b & 0xFF00) >> 8 | (b & 0x00FF) << 8;
   b = (b & 0xF0F0) >> 4 | (b & 0x0F0F) << 4;
   b = (b & 0xCCCC) >> 2 | (b & 0x3333) << 2;
   b = (b & 0xAAAA) >> 1 | (b & 0x5555) << 1;
   return b;
}

#define REPEAT2(a)  { {a}; {a}; }
#define REPEAT4(a)  { REPEAT2(a);  REPEAT2(a);  }
#define REPEAT8(a)  { REPEAT4(a);  REPEAT4(a);  }
#define REPEAT16(a) { REPEAT8(a);  REPEAT8(a);  }
#define REPEAT32(a) { REPEAT16(a); REPEAT16(a); }

typedef struct {
	int16_t x,y,dx,dy;
} TRect16;

typedef struct {
	uint8_t x,y;
} uint8_tp;

typedef struct {
	int16_t x,y;
} int16_tp;
