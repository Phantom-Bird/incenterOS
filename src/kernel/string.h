#pragma once

#include <stdint.h>

static inline char tolower(char c){
    return ('A' <= c && c <= 'Z')? (c - 'A' + 'a') : c;
}

uint32_t hex_to_uint(const char *str, uint64_t max_len);
int strncmp(const char *a, const char *b, uint64_t n);
int strcmp(const char *a, const char *b);
uint64_t strlen(const char *str);
char* strchr(const char *str, char c);
char* strrchr(const char *str, char c);
int __strcmp_before(const char *a, const char *b, char end);
