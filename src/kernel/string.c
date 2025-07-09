#include "string.h"

#define INF 0x3F3F3F3F3F3F3F3F

uint32_t hex_to_uint(const char *str, uint64_t max_len) {
    if (max_len == -1){
        max_len = INF;
    }

    uint32_t val = 0;
    for (uint64_t i = 0; i < max_len && str[i]; i++) {
        val = (val << 4) | (str[i] <= '9' ? str[i] - '0' : 
                            tolower(str[i]) - 'a' + 10);
    }
    return val;
}

int strncmp(const char *a, const char *b, uint64_t n){
    for (uint64_t i=0; i<n; i++){
        if (a[i] < b[i]){
            return -1;
        } else if (a[i] > b[i]){
            return 1;
        }
    }
    return 0;
}

int strcmp(const char *a, const char *b){
    for (uint64_t i=0;; i++){
        if (a[i] < b[i]){
            return -1;
        } else if (a[i] > b[i]){
            return 1;
        } else if (!a[i]){
            return 0;
        }
    }
}

uint64_t strlen(const char *str){
    for (uint64_t i=1;; i++){
        if (!*(str + i)){
            return i;
        }
    }
}

char* strchr(const char *str, char c){
    for (char *ptr = str; *ptr; ptr++){
        if (*ptr == c){
            return ptr;
        }
    }

    return 0;
}

char* strrchr(const char *str, char c){
    for (char *ptr = str + strlen(str) - 1; ptr >= str; ptr--){
        if (*ptr == c){
            return ptr;
        }
    }
    return 0;
}

int __strcmp_before(const char *a, const char *b, char end){
    for (uint64_t i=0;; i++){
        if (a[i] < b[i]){
            return -1;
        } else if (a[i] > b[i]){
            return 1;
        } else if (!a[i] || a[i] == end){
            return 0;
        }
    }
}
