#include <msx1hal.h>
#include <util.h>
//T_f start() { return (T_f)start; }

#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

T_f M0_menu();
T_f L0_levelInit();
T_f L1_levelMain();
T_f L1_levelDeath();
T_f L1_levelEnd();

static const uint8_t mySG[][8] = {

	{ 	0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000 },

	{ 	0b00111110,
		0b01000001,
		0b01011001,
		0b01000001,
		0b10000001,
		0b10111101,
		0b01000010,
		0b00111100 },

	{ 	0b00000000,
		0b00011100,
		0b00110010,
		0b01100001,
		0b11000001,
		0b11000010,
		0b01100100,
		0b00111000 },

	{ 	0b00000000,
		0b00011110,
		0b00110001,
		0b01100001,
		0b11000001,
		0b11000010,
		0b11000100,
		0b01111000 },


	{ 	0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000 },

	{ 	0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000 },
};


// BG is organized as | 1 | p0 | x0 | x1 | l0 | l1 | r0 | r1

static const uint8_t myBG0[4][8][2] = {

	{ 	{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b01000100, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00010001, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen } },

	{ 	{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00100000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen },
		{ 0b00000100, BBlack + FDarkGreen },
		{ 0b00010000, BBlack + FDarkGreen },
		{ 0b00000000, BBlack + FDarkGreen } },

	{ 	{ 0b01111110, BBlack + FDarkGreen },
		{ 0b11111111, BBlack + FDarkGreen },
		{ 0b11000011, BBlack + FDarkGreen },
		{ 0b11011011, BBlack + FDarkGreen },
		{ 0b11011011, BBlack + FDarkGreen },
		{ 0b11000011, BBlack + FDarkGreen },
		{ 0b11111111, BBlack + FDarkGreen },
		{ 0b01111110, BBlack + FDarkGreen } },

	{ 	{ 0b01111110, BBlack + FDarkGreen },
		{ 0b10100101, BBlack + FDarkGreen },
		{ 0b11011011, BBlack + FDarkGreen },
		{ 0b10100101, BBlack + FDarkGreen },
		{ 0b11011011, BBlack + FDarkGreen },
		{ 0b10100101, BBlack + FDarkGreen },
		{ 0b11011011, BBlack + FDarkGreen },
		{ 0b01111110, BBlack + FDarkGreen } },
};


static const uint8_t myBG1[4][8][2] = {

	{ 	{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed } },

	{ 	{ 0b00000000, BBlack + FDarkRed },
		{ 0b00100000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed },
		{ 0b00000100, BBlack + FDarkRed },
		{ 0b00010000, BBlack + FDarkRed },
		{ 0b00000000, BBlack + FDarkRed } },

	{ 	{ 0b01111110, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b01111110, BBlack + FDarkRed } },

	{ 	{ 0b01111110, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b10100101, BBlack + FDarkRed },
		{ 0b11011011, BBlack + FDarkRed },
		{ 0b01111110, BBlack + FDarkRed } },
};



typedef struct {
	int16_t x,y,dx,dy;
} TRect16;

typedef struct {
	uint8_t x,y;
} uint8_tp;

typedef struct {
	int16_t x,y;
} int16_tp;

// Maximum map size: {16*8,4*8} 

typedef struct {
	
	int16_tp pos;
	uint8_tp size;
	uint8_tp initPos;
	uint8_t tiles[24][128];
} TMap;

typedef struct {
	
	uint8_t enabled;
	uint16_t type;
	int16_tp pos, speed, acc;
	int8_t facing;
	int8_t state;
	int8_t step;
	TRect16 hitbox;
} TEntity;

typedef struct {
	
	uint32_t frameN;
	uint8_t jumpReleased;
	
	TMap map;
	TEntity entities[32];
} TLevelState;

TLevelState levelState;

enum { T_PLAYER };
enum { ST_RESTING, ST_JUMP0, ST_JUMP1, ST_JUMP2 };
enum { LEFT=0x1, RIGHT=0x2, TOP=0x4, BOTTOM=0x8};

T_f start() {

	setTMS9918_setMode2();
	setTMS9918_activatePage0();

	{
		T_SG SG;
		uint8_t i,j;
		for (j=0; j<5; j++)
			for (i=0; i<8; i++)
				SG[j][i] = mySG[j][i];
				
		for (j=0; j<127; j++)
			for (i=0; i<8; i++)
				SG[j+128][i] = reverse8(mySG[j][i]);
				
		setTMS9918_write(ADDRESS_SG,(uint8_t *)SG,sizeof(T_SG));
	}

	{
		T_PG PG;
		T_CT CT;
		uint8_t nt, h, cl, cr, l;
		for (nt=0; nt<3; nt++) {
			for (h=0; h<4; h++) {
				for (cl=0; cl<4; cl++) {
					for (cr=0; cr<4; cr++) {
						for (l=0; l<8; l++) {
							PG[nt][0x80+(h<<4)+(cl<<2)+(cr<<0)][l] = 
								(myBG0[cl][l][0]<<(h*(cl<2?1:2))) | (myBG0[cr][l][0]>>(8-h*(cr<2?1:2)));
							CT[nt][0x80+(h<<4)+(cl<<2)+(cr<<0)][l] = 
								max(myBG0[cl][l][1], myBG0[cr][l][1]);
							PG[nt][0xC0+(h<<4)+(cl<<2)+(cr<<0)][l] = 
								(myBG1[cl][l][0]<<(h*(cl<2?1:2))) | (myBG1[cr][l][0]>>(8-h*(cr<2?1:2)));
							CT[nt][0xC0+(h<<4)+(cl<<2)+(cr<<0)][l] = 
								max(myBG1[cl][l][1], myBG1[cr][l][1]);
						}
					}
				}
			}
		}

		setTMS9918_write(ADDRESS_PG,&PG[0][0][0],sizeof(T_PG));		
		setTMS9918_write(ADDRESS_CT,&CT[0][0][0],sizeof(T_CT));		
	}

	return (T_f)(M0_menu);
}

T_f M0_menu() {	

	return (T_f)(L0_levelInit);
}

const char mapInfo[] = 
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"a                                                                                                                              a"
"a                                                                                                                              a"
"a                                                                                                                              a"
"                                                                                                                               a"
"                     aaaaaaa                                                                                                   a"
"                                                                                                                               a"
"a                                                                                                                              a"
"a                                                                                                                              a"
"a                     aaaaaa                                                                                                   a"
"a   aaaaa a                                                                                                                    a"
"a                                                                                                                              a"
"a                                                                                                                              a"
"a              aaaaa a                                                                                                         a"
"a                                                                                                                              a"
"a     aaaaa           a a aa  aaa        aaaa                                                                                  a"
"a                                a   a       a                                                                                 a"
"a            aaaaaa                aaa       a                                                                                 a"
"a                               a                                                                                              a"
"a                        a  a             a a                                                                                  a"
"a    aaaaa                a    a          a                                                                                    a"
"a                                        aa      aaaa                                                                          a"
"a                                 aaaa                                                                                         a"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa    aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

T_f L0_levelInit() {
	
	TEntity *player = &levelState.entities[0];
	
	levelState.map.size.x=128;
	levelState.map.size.y=24;

	levelState.map.initPos.x=2;
	levelState.map.initPos.y=2;
	
	levelState.map.pos.x=0x0000;
	levelState.map.pos.y=0x0000;
	
	{
		uint8_t i8;
		for (i8=0; i8<32; i8++)
			levelState.entities[i8].enabled = 0;
	}
	levelState.frameN = 0;
	levelState.jumpReleased = 0;

	player->enabled = 1;
	player->type = T_PLAYER;
	player->pos.x = 0x400;
	player->pos.y = 0x400;
	player->speed.x = 0x0;
	player->speed.y = 0x80;
	player->acc.x = 0x0;
	player->acc.y = 0x0;
	player->facing = 0x1;
	player->state = ST_JUMP0;
	player->step = 0;
	player->hitbox.x = 0x20;
	player->hitbox.y = 0x20;
	player->hitbox.dx = 0xDF;
	player->hitbox.dy = 0xDF;
	
	{


		uint8_t i,j;
		for (i=0; i<levelState.map.size.y; i++) {
			for (j=0; j<levelState.map.size.x; j++) {
				switch(mapInfo[i*128+j]) {
				case 'a':
					levelState.map.tiles[23-i][j] = 2;
					break;
				default:
					levelState.map.tiles[23-i][j] = 0;
				}
			}
		}
	}

	return (T_f)(L1_levelMain);
}

int nFrame;
int nSkipped;

		
T_SA SA;
T_PN PN;

T_f L1_levelMain() {

//	printf("levelMain\n");

	uint8_t x0,x1,y0,y1;
	uint8_t x0r,x1r,y0r,y1r;
	
	uint8_t colVec, colResponse;

	TMap *map = &levelState.map;	
	TEntity *player = &levelState.entities[0];
	if (player->enabled) {
		
		uint8_t keys = keyboard_read();

		if (player->speed.x>=0) player->acc.x = max(-player->speed.x,-0x2);
		if (player->speed.x<=0) player->acc.x = min(-player->speed.x, 0x2);
		
		
		
		if (keys & KEYBOARD_RIGHT) {
//			if (player->facing>=0)
				player->acc.x=0x4;
//			else
//				player->acc.x=0x2;
		}

		if (keys & KEYBOARD_LEFT) {
//			if (player->facing<=0)
				player->acc.x=-0x4;
//			else
//				player->acc.x=-0x2;
		}

		if (keys & KEYBOARD_SPACE) {
			if (player->state<ST_JUMP0 && levelState.jumpReleased) {
				player->speed.y = 0x74;
				player->state++;
				levelState.jumpReleased = 0;
			} else {
				player->acc.y = -0x5;
			}
		} else {
			player->acc.y=-0x12;
			levelState.jumpReleased = 1;
		}
		
		player->speed.x += player->acc.x;
		player->speed.y += player->acc.y;
		player->speed.x = cropped(player->speed.x,-0x40,0x40);
		player->speed.y = cropped(player->speed.y,-0xFF,0xA0);
		player->pos.x += player->speed.x;
		player->pos.y += player->speed.y;
	

		if (player->pos.y<0) 
			return (T_f)(L1_levelDeath);

		if (player->pos.x<0) 
			return (T_f)(L1_levelEnd);
	
		x0 =  (player->pos.x + player->hitbox.x)>>8;
		x1 =  (player->pos.x + player->hitbox.x + player->hitbox.dx)>>8;
		y0 =  (player->pos.y + player->hitbox.y)>>8;
		y1 =  (player->pos.y + player->hitbox.y + player->hitbox.dy)>>8;

		x0r =  (player->pos.x + player->hitbox.x)&0xFF;
		x1r =  0xFF - ((player->pos.x + player->hitbox.x + player->hitbox.dx)&0xFF);
		y0r =  (player->pos.y + player->hitbox.y)&0xFF;
		y1r =  0xFF - ((player->pos.y + player->hitbox.y + player->hitbox.dy)&0xFF);
		
		colVec = 0;
		if (map->tiles[y0][x0]>0) colVec +=0x1;
		if (map->tiles[y0][x1]>0) colVec +=0x2;
		if (map->tiles[y1][x0]>0) colVec +=0x4;
		if (map->tiles[y1][x1]>0) colVec +=0x8;
		
		colResponse = 0;
		
		switch (colVec) {
		case 0x01: if (x0r>y0r) colResponse = LEFT;  else colResponse = BOTTOM;	break;
		case 0x02: if (x1r>y0r) colResponse = RIGHT; else colResponse = BOTTOM;	break;
		case 0x04: if (x0r>y1r) colResponse = LEFT;  else colResponse = TOP;    break;
		case 0x08: if (x1r>y1r) colResponse = RIGHT; else colResponse = TOP;    break;
			
		case 0xF-0x1: colResponse = RIGHT+TOP;    break;
		case 0xF-0x2: colResponse = LEFT +TOP;    break;
		case 0xF-2-4: case 0xF-0x4: colResponse = RIGHT+BOTTOM; break;
		case 0xF-1-8: case 0xF-0x8: colResponse = LEFT +BOTTOM; break;

		case 0x1+0x2: colResponse = BOTTOM; break;
		case 0x4+0x8: colResponse = TOP;    break;
		case 0x1+0x4: colResponse = LEFT;   break;
		case 0x2+0x8: colResponse = RIGHT;  break;
		
		default:
			break;
		}
		
		if ( colResponse & BOTTOM ) { // Collision below
			
			player->state = ST_RESTING;
			player->pos.y = ((uint16_t)(y0+1)<<8) - player->hitbox.y;			
			player->speed.y = 0;
		}
		
		if ( colResponse & TOP ) { // Collision above
			
			player->pos.y = ((uint16_t)(y1)<<8) - player->hitbox.y - player->hitbox.dy;			
			player->speed.y = 0;
		}

		if ( colResponse & LEFT ) { // Collision left
			
			player->speed.x += 0x08; 
			player->pos.x = ((uint16_t)(x0+1)<<8) - player->hitbox.x;			
		} 
		if ( colResponse & RIGHT ) { // Collision left
			
			player->speed.x -= 0x08; 
			player->pos.x = ((uint16_t)(x1)<<8) - player->hitbox.x - player->hitbox.dx;			
		}
		
	}
	
	player->facing = 0;
	if (player->speed.x<0) player->facing=-1;
	if (player->speed.x>0) player->facing= 1;

	

	//if (nFrame++ % 2 == 0) {
		
	{

		int idealMapSpeed = (player->pos.x-((0xF8-player->facing*0x40)<<4) - map->pos.x)>>4;
		if (player->speed.x>0) {
			if (idealMapSpeed>0x10) {
				map->pos.x += player->speed.x;
			}
			if (idealMapSpeed>0) {
				map->pos.x += player->speed.x;
			}
		} else {
			if (idealMapSpeed<-0x10) {
				map->pos.x += player->speed.x;
			}
			if (idealMapSpeed<0) {
				map->pos.x += player->speed.x;
			}
		}
		
		map->pos.x = cropped(map->pos.x,0,((map->size.x-32)<<8)-1);
		
	}

	{		
		uint16_t displayMapPosX = ((map->pos.x+0x20)>>6)<<6;
		uint16_t displayMapPosY = 0;
		
		uint16_t spritePosX = (player->pos.x+0x10-displayMapPosX)>>5;


		nSkipped++;
		if (nSkipped<0) {
			if (spritePosX < SA[0].x && player->speed.x>0) return (T_f)(L1_levelMain);
			if (spritePosX > SA[0].x && player->speed.x<0) return (T_f)(L1_levelMain);
		}
		nSkipped=0;
		
		SA[0].x =           spritePosX;
		SA[0].y = 20*8-8-1-((player->pos.y+0x10-map->pos.y)>>5);

		SA[0].pattern = 1;
		SA[0].color = BDarkYellow;


		nFrame++;
		if (player->acc.x>0) SA[0].pattern = 0x02+((nFrame/3)%2);
		if (player->acc.x<0) SA[0].pattern = 0x82+((nFrame/3)%2);
		
		{
			uint8_t i,j;
			uint8_t x2=(displayMapPosX+0x20)>>6;
			uint8_t pv = 0x80 + ((x2&3)<<4);
			for (i=0; i<20; i++) {
				uint8_t *p = &map->tiles[19-i][(x2>>2)];
				uint8_t old = *p++;
				for (j=0; j<TILE_WIDTH; j++) {
					PN[i][j]= pv + (old<<2) + (old = *p++);
				}
			}	
		}
		setTMS9918_write(ADDRESS_PN0,&PN[0][0],sizeof(PN));		
		setTMS9918_write(ADDRESS_SA0,(uint8_t *)SA,sizeof(SA));				
	}
	
	return (T_f)(L1_levelMain);
}

T_f L1_levelDeath() {

	//std::cout << "Death" << std::endl;
	return (T_f)(M0_menu);
}

T_f L1_levelEnd() {

	//std::cout << "Goal reached!" << std::endl;
	return (T_f)(M0_menu);
}
