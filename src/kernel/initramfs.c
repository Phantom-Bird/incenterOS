#include "initramfs.h"
#include "cpio.h"
#include "string.h"
#include "mem.h"
#include "print.h"
#include "pool.h"

#define INITRAMFS_POOL_VIRT      0xFFFF800000000000
#define INITRAMFS_POOL_MAX       (1 << 30) // 1GB
#define INITRAMFS_POOL_INIT_SIZE (1 << 24) // 16MB

FSItem *fs_root;
LargePool fs_pool;

static inline FSItem* new_item(FSItem *parent){    
    FSItem *item = pool_alloc(&fs_pool, sizeof(FSItem));

    if (!item){
        return item;
    }

    memset(item, 0, sizeof(FSItem));
    item->parent = parent;
    item->next = parent->first_child;
    parent->first_child = item;

    return item;
}

void initramfs_init(){
    fs_pool = create_pool(INITRAMFS_POOL_VIRT, INITRAMFS_POOL_INIT_SIZE, INITRAMFS_POOL_MAX);
    fs_root = new_item(NULL);
}

FSItem* find_child(FSItem *parent, const char *name){
    if (strcmp(name, ".") == 0){
        return parent;
    }
    if (strcmp(name, "..") == 0){
        return parent->parent;
    }

    for (FSItem *child = parent->first_child; child; child = child->next){
        if (__strcmp_before(child->name, name, '/') == 0){
            return child;
        }
    }

    return NULL;
}

FSItem* insert_child(FSItem *parent, const char *name, CPIOHeader *hdr, void *file_data) {
    FSItem *child = find_child(parent, name);
    if (child){
        return child;
    }

    // 获取文件头信息
    uint64_t file_size = hex_to_uint(hdr->filesize, 8);
    uint64_t mode = hex_to_uint(hdr->mode, 8);

    child = new_item(parent);
    child->flags |= (mode & MODE_DIR)? ITEM_DIR : 0;
    child->flags |= (mode & MODE_FILE)? ITEM_FILE : 0;
    child->name = pool_alloc_copy(&fs_pool, strlen(name)+1, name);
    child->file_data = file_data;
    child->file_size = file_size;

    return child;
}

FSItem* initramfs_find(FSItem *work_dir, const char *path){
    FSItem *cur = work_dir;

    if (path[0] == '/'){
        path++;
        cur = fs_root;
    }
    
    char *split;
    do {
        if (!cur){
            return NULL;
        }

        cur = find_child(cur, path/*会自动以'/'结束*/);
        split = strchr(path, '/');
        path = split + 1;
    } while (split);

    return cur;
}

FSItem* initramfs_create(FSItem *work_dir, const char *path, CPIOHeader *hdr, void *file_data){
    FSItem *cur = work_dir;

    if (path[0] == '/'){
        path++;
        cur = fs_root;
    }
    
    char *split;
    while (split = strchr(path, '/')) {
        if (!cur){
            raise_err(path);
            return NULL;
        }

        cur = find_child(cur, path/*会自动以'/'结束*/);
        path = split + 1;
    }

    return insert_child(cur, path, hdr, file_data);
}