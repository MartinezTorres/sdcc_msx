#pragma once
#include <msxhal.h>
#ifndef SL_SPLITS
    #define SL_SPLITS 16
#endif
#ifndef SL_MAX_SIZE
    #define SL_MAX_SIZE (256-SL_SPLITS-1)
#endif

typedef uint8_t SL_ITER;

typedef struct {
    
    SL_ITER next[SL_MAX_SIZE+SL_SPLITS+1];
    SL_ITER prev[SL_MAX_SIZE+SL_SPLITS+1];
    SL_ITER free[SL_MAX_SIZE];
    uint8_t size;
} SplitList;

void SL_Init(SplitList *data);

INLINE uint8_t SL_Size(SplitList *data) { data; return data->size; }

INLINE uint8_t SL_Get(SplitList *data, SL_ITER i) { data; return i-(1+SL_SPLITS); }

INLINE SL_ITER SL_Next(SplitList *data, SL_ITER i) { data;return data->next[i]; }

INLINE SL_ITER SL_End(SplitList *data, uint8_t split) { data; return 1+split; }

INLINE SL_ITER SL_Begin(SplitList *data, uint8_t split) { data; return split; }

INLINE void SL_MoveToSplit(SplitList *data, uint8_t i, uint8_t split) { 
	
    {
	uint8_t pi = data->prev[i], ni = data->next[i];
	data->next[pi] = ni;
	data->prev[ni] = pi;
    }
	
    {
        uint8_t *pni = &data->next[split];
        uint8_t ni = *pni;
		
	data->prev[i] = split;
        data->next[i] = ni;
        
        data->prev[ni] = i;
        *pni = i;
    }
}

void SL_Erase(SplitList *data, SL_ITER i);

SL_ITER SL_New(SplitList *data, uint8_t split);
