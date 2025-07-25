#pragma once

#include <stdint.h>
#include "cpio.h"
#include "kmalloc.h"

typedef uint64_t Flags;
#define HASH_BUCKETS 56

enum {
    ITEM_FILE = 1 << 0,
    ITEM_DIR  = 1 << 1
};

typedef struct FSItem {
    Flags flags;
    char *name;

    uint64_t bucket;  // 对齐
    struct FSItem *parent;
    struct FSItem *first_child[HASH_BUCKETS];
    struct FSItem *next;
    
    uint64_t file_size;
    void *file_data;
} FSItem;


static const uint64_t IT_SHOULD_BE_512 = (sizeof(FSItem) + sizeof(KMallocHeader));

// 全局根节点
extern FSItem *fs_root;

void initramfs_init();
FSItem* initramfs_find(FSItem *work_dir, const char *path);
FSItem* initramfs_create(FSItem *work_dir, const char *path, CPIOHeader *hdr, void *file_data);
