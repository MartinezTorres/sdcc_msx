#include <msxhal.h>
#include "split_list.h"

void SL_Init(SplitList *data) { 

    memset(data, 0, sizeof(SplitList));
    {
        uint8_t i;
        for (i = 0; i < SL_MAX_SIZE; i++)
            data->free[i] = i+1+SL_SPLITS;

        for (i = 0; i < 1+SL_SPLITS; i++) {
            data->next[i] = i + 1;
            data->prev[i + 1] = i;
        }
    }
}

void SL_MoveToSplit(SplitList *data, uint8_t i, uint8_t split) { 
	
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

void SL_Erase(SplitList *data, SL_ITER i) {

    {
        uint8_t sz = data->size--;
        data->free[sz] = i;
    }
    
    {
        uint8_t pi = data->prev[i], ni = data->next[i];
        data->next[pi] = ni;
        data->prev[ni] = pi;
    }
}

SL_ITER SL_New(SplitList *data, uint8_t split) {

    uint8_t p = data->free[data->size++];
    data->prev[p] = split;
    {
        uint8_t *pni = &data->next[split];
        uint8_t ni = *pni;
        data->next[p] = ni;
        data->prev[ni] = p;
        *pni = p;
    }
    return p;
}

