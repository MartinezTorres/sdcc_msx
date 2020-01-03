#pragma once
#include <msxhal.h>


#define SplitListIdx(CAPACITY, SPLITS)          \
struct {                                        \
    uint8_t next[CAPACITY+SPLITS+1];            \
    uint8_t prev[CAPACITY+SPLITS+1];            \
    uint8_t free[CAPACITY];                     \
    uint8_t size;                               \
}

#define SplitList_T(CAPACITY, SPLITS, DATA_T)   \
struct {                                        \
    uint8_t next[CAPACITY+SPLITS+1];            \
    uint8_t prev[CAPACITY+SPLITS+1];            \
    uint8_t free[CAPACITY];                     \
    uint8_t size;                               \
    DATA_T data[CAPACITY];                      \
}

#define SL_GetCapacity(T) (sizeof((T).free))
#define SL_GetSplits(T) ((sizeof((T).next)-sizeof((T).free)-1))
#define SL_Size(T) ((T).size)
#define SL_GetIdx(T, i) ((i)-(1+SL_GetSplits(T)))
#define SL_Get(T, i) ((T).data[SL_GetIdx(T,(i))])

#define SL_SplitNext (T, i) ((T).next[(i)])
#define SL_SplitBegin(T, split) SL_SplitNext(T,split)
#define SL_SplitEnd  (T, split) (1+(split))

#define SL_AllNext (T, i) SL_AllNext_Int(i, SL_AllEnd())
INLINE uint8_t SL_AllNext_Int(uint8_t i, uint8_t endMark) { do i++; while (i<endMark); return i; }
#define SL_AllBegin(T) SL_AllNext(T,0)
#define SL_AllEnd  (T) (1+SL_GetSplits(T))

#define SL_Init(T) SL_Init_Int(T.next, T.prev, T.free, T.size, SL_GetCapacity(T), SL_GetSplis(T))
INLINE void SL_Init_Int(uint8_t *next, uint8_t *prev, uint8_t *free, uint8_t *size, const uint8_t capacity, const uint8_t splits) {

    uint8_t i;
    for (i = 0; i < capacity; i++)
        free[i] = i+1+splits;

    for (i = 0; i < 1+splits; i++) {
        next[i] = i + 1;
        prev[i + 1] = i;
    }
    *size = 0;
}

#define SL_MoveToSplit(T, i, split) SL_MoveToSplit_Int(T.next, T.prev, i, split)
INLINE void SL_MoveToSplit_Int(uint8_t *next, uint8_t *prev, uint8_t i, uint8_t split) {
    
    {                                                                  
        uint8_t pi = prev[i], ni = next[i];                        
        next[pi] = ni;                                               
        prev[ni] = pi;                                               
    }                                                                  
	                                                                   
    {                                                                  
        uint8_t *pni = &next[split];                                 
        uint8_t ni = *pni;                                             
		                                                               
        prev[i] = split;                                             
        next[i] = ni;                                                
                                                                       
        prev[ni] = i;                                                
        *pni = i;                                                      
    }                                                                  
}

#define SL_Erase(T, i) SL_Erase_Int(T.next, T.prev, T.free, T.size, i)
INLINE void SL_Erase_Int(uint8_t *next, uint8_t *prev, uint8_t *free, uint8_t *size, uint8_t i) {

    {	                                                               
        uint8_t sz = *size;
        (*size)--;	                                       
        free[sz] = i;	                                               
    }	                                                               
    	                                                               
    {	                                                               
        uint8_t pi = prev[i], ni = next[i];	                       
        next[pi] = ni;	                                           
        prev[ni] = pi;	                                           
    }	                                                               
}

#define SL_New(T, split) SL_New_Int(T.next, T.prev, T.free, T.size, split)
INLINE uint8_t SL_New_Int(uint8_t *next, uint8_t *prev, uint8_t *free, uint8_t *size, uint8_t split) {

    uint8_t p = free[*size];
    (*size)++;
    prev[p] = split;
    {
        uint8_t *pni = &next[split];
        uint8_t ni = *pni;
        next[p] = ni;
        prev[ni] = p;
        *pni = p;
    }
    return p;
}
