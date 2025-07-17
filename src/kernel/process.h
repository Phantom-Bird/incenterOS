#pragma once

#include <stdint.h>
#include <stddef.h>
#include "paging.h"
#include "kmalloc.h"
#include "save_context.h"

typedef struct Process {
    // 基本信息
    uint32_t pid;                   // 进程ID
    uint64_t ppid;                  // 父进程ID
    uint64_t state;                 // 进程状态
    uint64_t priority;              // 调度优先级
      
    // 寄存器
    SavedRegisters context;
    // 特殊寄存器
    uint64_t rsp_init;              // 栈指针
    uint64_t rip_init;              // 指令指针
    PhysicalAddress cr3;            // 页目录地址

    // 内存管理
    struct ProcessPage *pages;  // 占用页链表

    // 进程调度
    struct process *next;       // 下一个进程
} Process;

typedef struct __attribute__((packed)) ProcessPage  {
    uint16_t flags;
    PhysicalAddress page;
    struct ProcessPage *next;
} ProcessPage;

enum PageFlags {
    PAGE_TABLE  = 1 << 0,
    STACK       = 1 << 1,
    SHARED      = 1 << 2,
};

static const uint64_t IT_SHOULD_BE_32 = sizeof(ProcessPage) + sizeof(KMallocHeader);

enum ProcessStatus {
    OK                   = 0,
    FAILED_TO_ALLOC_PAGE = 1,
    INVALID_ELF          = 2,
    INVALID_ADDRESS      = 3,
}; 
