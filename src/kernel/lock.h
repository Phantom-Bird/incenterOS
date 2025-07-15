#pragma once

#include <stdint.h>

typedef volatile uint8_t Lock;

static inline void acquire_lock(Lock *lock){
    __asm__ volatile (
        "spin_lock:"
            "movb $1, %%al;"
            "xchgb %%al, (%0);"  // 原子交换操作
            "testb %%al, %%al;"
            "jz acquired;"
            "pause;"  // 优化
            "jmp spin_lock;"
        "acquired:"
        : 
        : "r"(lock)
        : "memory", "al"
    );
}

static inline void release_lock(Lock *lock){
    __asm__ volatile (
        "mfence;"  // 防止访问顺序被重排
        "movb $0, %%al;"
        "xchgb %%al, (%0);"
        :
        : "r"(lock)
        : "memory", "al"
    );
}

#define WITH_LOCK(lock_ptr, stmt) { acquire_lock(lock_ptr); stmt; release_lock(lock_ptr); }