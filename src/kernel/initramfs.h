#pragma once

#include <stdint.h>
#include "cpio.h"

typedef uint64_t Flags;

enum {
    ITEM_FILE = 1 << 0,
    ITEM_DIR  = 1 << 1
};

typedef struct FSItem {
    Flags flags;
    char *name;

    struct FSItem *parent;
    struct FSItem *first_child;
    struct FSItem *next;
    
    uint64_t file_size;
    void *file_addr;
} FSItem;

// 全局根节点
extern FSItem *fs_root;

void initramfs_init();
FSItem* initramfs_find(FSItem *work_dir, const char *path);
FSItem* initramfs_create(FSItem *work_dir, const char *path, CPIOHeader *hdr, void *file_addr);
