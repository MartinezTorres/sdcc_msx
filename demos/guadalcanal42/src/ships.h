#pragma once
#include "guadalcanal.h"

typedef struct {
	const uint8_t rows, cols;
	const char *shape;
	const char *className;
	void (*onHit)(void); 
	void (*onDestroy)();
	void (*onTurn)();
} 
T_ShipClass;

typedef enum {
    SHIP_TUG,
    SHIP_DESTROYER,
    SHIP_CRUISER,
    SHIP_BATTLESHIP
} 
E_ShipClass;

extern const T_ShipClass ShipClasses[];

typedef struct {
    
    uint8_t rows, cols, orientation;
    E_ShipClass shipClass;
    
} T_ShipStatus;

#define MAX_SHIP_COUNT 10

typedef struct {    
    uint8_t rows, cols;
    T_ShipStatus shipStatus[MAX_SHIP_COUNT];
    
    
} T_BattleFieldStatus;

//bool tryFindBoats(T_ShipDistribution *shipDistribution) {} 
