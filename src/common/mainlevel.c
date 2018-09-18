#include <capetsrace.h>

extern void fillFrameBuffer(uint8_t tiles[24][128], uint8_t PNaddressH, uint16_t x, uint16_t y);

extern T_f M0_menu();

T_f L0_levelInit();
T_f L1_levelMain();
T_f L1_levelDeath();
T_f L1_levelEnd();

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

T_f start() {

	TMS9918_setRegister(7,BBlack+FWhite);
	TMS9918_setMode2(FALSE);
	
	initHeadSprites(hairSprite, headSprite);
	
	
	return (T_f)(M0_menu);
}
/*
 * 
static void findFreeTiles(uint8_t freeTiles[256], const TMap *map) {
	
	{
		uint8_t i = 255; //256;
		do freeTiles[i]=1; while (--i);
	}
	
	{
		uint8_t i = map->size.y, j = map->size.x-1;
		while (i--) {
			const uint8_t *t = map->tiles[i];
			while (j--) {
				freeTiles[(t[j+1]<<3) + t[j+1]] = 0;
				freeTiles[(t[j]<<3) + t[j+1]] = 0;
				freeTiles[(t[j]<<3) + t[j]] = 0;
			}
		}
	}

	{
		uint8_t i = 255; //256;
		do freeTiles[i]=freeTiles[i&0x3F]; while (--i);
	}
}


T_f L0_levelInit() {
	
	TEntity *player = &levelState.entities[0];

	TMS9918_setMode2(FALSE);
	TMS9918_setRegister(7,BBlack+FWhite);

	{
		TMS9918Register TMS9918Status;
		
		TMS9918Status.reg[1] = 0;
		
		TMS9918Status.magnifySprites = 1;
		TMS9918Status.sprites16 = 0;
		TMS9918Status.blankScreen = 1;
		TMS9918Status.generateInterrupts = 1;
		TMS9918Status.mem416K = 1;
	
		TMS9918_setRegister(1,TMS9918Status.reg[1]);
	}

	TMS9918_activatePage0();

	{
		uint8_t c[8];
		uint8_t i,j;
		for (j=0; j<5; j++)
			TMS9918_memcpy(ADDRESS_SG+(j<<3),&mySG[j][0],8);
				

		for (j=0; j<127; j++) {
			for (i=0; i<8; i++)
				c[i] = reverse8(mySG[j][i]);
			TMS9918_memcpy(ADDRESS_SG+((128+j)<<3),&c[0],8);
		}	
	}

	initTilemap();
	
	
	
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
				case 'b':
					levelState.map.tiles[23-i][j] = 3;
					break;
				case '#':
					levelState.map.tiles[23-i][j] = 1;
					break;
				default:
					levelState.map.tiles[23-i][j] = 0;
				}
			}
		}
	}


	{
		uint8_t freeTiles[256];
		findFreeTiles(freeTiles, &levelState.map);
		initBloodMeterTiles(freeTiles);
		initASCIITiles(' ', freeTiles);
	}


	return (T_f)(L1_levelMain);
}

int nFrame;
int nSkipped;
	
T_SA SA;
uint8_t statusBar[16];
static void drawStatusBar(uint8_t yCoord, uint16_t PNaddressH) {
	
	{
		uint8_t *p = statusBar;
		uint8_t v = ascii2tiles[0x18];
		REPEAT16( *p++ = v; );
	}
	
	{
		uint8_t b = 127-(((nFrame)>>4)&0x7F);
		uint8_t c = b>>3;
		uint8_t v = ascii2tiles[0x10];
		if (c < 16)
			statusBar[c++] = ascii2tiles[0x10 + (b&7)];
		while (c < 16) statusBar[c++] = v;
	}
	
	TMS9918_memcpy((PNaddressH<<8) + (yCoord<<5) + 16,statusBar,sizeof(statusBar));		
}

T_f L1_levelMain() {

	uint8_t x0,x1,y0,y1;
	uint8_t x0r,x1r,y0r,y1r;
	
	uint8_t colVec, colResponse;

	TMap *map = &levelState.map;	
	TEntity *player = &levelState.entities[0];

	TMS9918_setRegister(7,BGray+FWhite);

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
		player->speed.y = cropped(player->speed.y,-0xC0,0xA0);
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

	TMS9918_setRegister(7,BDarkRed+FWhite);
	
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
		if (player->acc.x>0) SA[0].pattern = 0x02+((nFrame>>2)&0x1);
		if (player->acc.x<0) SA[0].pattern = 0x82+((nFrame>>2)&0x1);

	TMS9918_setRegister(7,BDarkYellow+FWhite);
		
		fillFrameBuffer(map->tiles,ADDRESS_PN0>>8,displayMapPosX,displayMapPosY);

	TMS9918_setRegister(7,BDarkGreen+FWhite);
	
		drawStatusBar(20,ADDRESS_PN0>>8);	
	
	TMS9918_setRegister(7,BGray+FWhite);
	
		TMS9918_memcpy(ADDRESS_SA0,(uint8_t *)SA,sizeof(SA));						
	}
	
	TMS9918_setRegister(7,BBlack+FWhite);
	
	return (T_f)(L1_levelMain);
}

T_f L1_levelDeath() {

	return (T_f)(M0_menu);
}

T_f L1_levelEnd() {

	return (T_f)(M0_menu);
}
*/
