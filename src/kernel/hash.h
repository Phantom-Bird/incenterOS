#pragma once

#include "stdint.h"

static inline uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    while (*str) {
        hash = (hash << 5) + hash + *str;  // hash * 33 + c
        str++;
    }
    return hash;
}

static inline uint32_t hash_string_mod(const char *str, uint32_t mod) {
    return hash_string(str) % mod;
}

// 仅文件名转用

static inline uint32_t __hash_string_before(const char *str, char delim) {
    uint32_t hash = 5381;
    while (*str && *str != delim) {
        hash = (hash << 5) + hash + *str;  // hash * 33 + c
        str++;
    }
    return hash;
}

static inline uint32_t __hash_string_mod_before(const char *str, uint32_t mod, char delim) {
    return __hash_string_before(str, delim) % mod;
}