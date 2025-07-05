#include "bitarray.h"

void set_bit(bit_array arr, size_t idx){
    arr[idx / 8] |= 1 << (idx % 8);
}

void clear_bit(bit_array arr, size_t idx){
    arr[idx / 8] &= ~(1 << (idx % 8));
}

int test_bit(bit_array arr, size_t idx){
    return (arr[idx / 8] >> (idx % 8)) & 1;
}