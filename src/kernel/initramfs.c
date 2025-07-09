#include "initramfs.h"
#include "cpio.h"
#include "string.h"
#include "mem.h"
#include "print.h"

#define MAX_FILE 1024

FSItem *fs_root;
FSItem fs_pool[MAX_FILE];
int pool_idx;
char a[sizeof(FSItem)];

static inline FSItem* new_item(FSItem *parent){
    if (pool_idx >= MAX_FILE){
        raise_err("Cannot insert the file");
    }
    
    FSItem *ret = fs_pool + pool_idx;
    pool_idx++;

    memset(ret, 0, sizeof(FSItem));
    ret->parent = parent;
    ret->next = parent->first_child;
    parent->first_child = ret;

    return ret;
}

void initramfs_init(){
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

FSItem* insert_child(FSItem *parent, const char *name, CPIOHeader *hdr, void *file_addr) {
    // WARNING: name 必须为全局变量/static/malloc

    // 获取文件头信息
    uint64_t file_size = hex_to_uint(hdr->filesize, 8);
    uint64_t mode = hex_to_uint(hdr->mode, 8);

    FSItem *child = new_item(parent);
    child->flags |= (mode & MODE_DIR)? ITEM_DIR : 0;
    child->flags |= (mode & MODE_FILE)? ITEM_FILE : 0;
    child->name = name;
    child->file_addr = file_addr;
    child->file_size = file_size;

    return child;
}

FSItem* initramfs_find(FSItem *work_dir, const char *path){
    FSItem *cur = work_dir;

    if (path[0] == '/'){
        path++;
        cur = fs_root;
    }
    
    char *split = strchr(path, '/');
    do {
        if (!cur){
            return NULL;
        }

        cur = find_child(cur, path/*会自动以'/'结束*/);
        path = split + 1;
    } while (split);

    return cur;
}

FSItem* initramfs_create(FSItem *work_dir, const char *path, CPIOHeader *hdr, void *file_addr){
    FSItem *cur = work_dir;

    if (path[0] == '/'){
        path++;
        cur = fs_root;
    }
    
    char *split = strchr(path, '/');
    while (split) {
        if (!cur){
            return NULL;
        }

        cur = find_child(cur, path/*会自动以'/'结束*/);
        path = split + 1;
    }

    return insert_child(cur, path, hdr, file_addr);
}