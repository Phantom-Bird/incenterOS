#include "mem.h"

void memcpy(uint8_t *to, uint8_t *from, uint64_t bytes){
    for (int i=0; i<bytes; i++){
        *(to + i) = *(from + i);
    }
}

void memset(uint8_t *start, uint8_t val, uint64_t bytes){
    for (int i=0; i<bytes; i++){
        *(start + i) = val;
    }
}
