#pragma once

#include <stdint.h>
#include <stddef.h>
#include "paging.h"
#include "kmalloc.h"

typedef struct Process {
    uint32_t pid;               // 进程ID
    uint64_t ppid;              // 父进程ID
    uint64_t state;             // 进程状态
    uint64_t priority;          // 调度优先级
    uint64_t rsp;               // 栈指针
    uint64_t rip;               // 指令指针
    PhysicalAddress cr3;        // 页目录地址
    uint64_t user_stack;        // 用户栈地址
    struct ProcessPage *pages;  // 占用页链表
    struct process *next;       // 下一个进程
} Process;

typedef struct ProcessPage{
    uint8_t is_shared;
    uint8_t is_pagetable;
    PhysicalAddress page;
    struct ProcessPage *next;
} ProcessPage;

static const uint64_t IT_SHOULD_BE_32 = sizeof(ProcessPage) + sizeof(KMallocHeader);

enum {
    OK                   = 0,
    FAILED_TO_ALLOC_PAGE = 1,
    INVALID_ELF          = 2,
    INVALID_ADDRESS      = 3,
}; 
