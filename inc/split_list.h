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

INLINE uint8_t SL_Size(SplitList *data) { return data->size; }

INLINE uint8_t SL_Get(SplitList *data, SL_ITER i) { UNUSED(data); return i-(1+SL_SPLITS); }

INLINE SL_ITER SL_Next(SplitList *data, SL_ITER i) { return data->next[i]; }

INLINE SL_ITER SL_Begin(SplitList *data, uint8_t split) { UNUSED(data); return split; }

INLINE SL_ITER SL_End(SplitList *data, uint8_t split) { UNUSED(data); return 1+split; }

void SL_MoveToSplit(SplitList *data, uint8_t i, uint8_t split);

void SL_Erase(SplitList *data, SL_ITER i);

SL_ITER SL_New(SplitList *data, uint8_t split);
