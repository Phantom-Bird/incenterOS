#include "cpio.h"
#include "print.h"
#include "string.h"
#include "initramfs.h"

// 单个 CPIO 条目结构：
// struct CPIO __attribute__((aligned(4))){
//     CPIOHeader header;
//     byte filename[header.namesize];
//     byte filedata[header.filesize];
// };

static inline uint64_t align_up(uint64_t x){
    return (x + 3ull) & ~3ull;
}

static inline void* next_pos(void *base, uint64_t size){
    return (void*)align_up((uint64_t)base + size);
}

const char end[] = "TRAILER!!!";

void cpio_init(void *archive, uint64_t cpio_size) {
    CPIOHeader *header = archive;
    initramfs_init();
    
    while (1) {
        if (header > archive + cpio_size){
            raise_err("[ERROR] Out of CPIO file");
        }

        if (strncmp(header->magic, "070701", 6) != 0){
            error_color();
            print("[ERROR] Invalid CPIO magic at off=");
            print_hex(header->magic - (char*)archive);
            raise_err("");
        }
            
        uint32_t namesize = hex_to_uint(header->namesize, 8);
        uint32_t filesize = hex_to_uint(header->filesize, 8);
        char *filename = (char*)(header + 1);
        
        // 结束标记：文件名为 "TRAILER!!!"
        if (strcmp(filename, end) == 0){
            break;
        }
        
        // 文件数据位置（对齐 4B）
        void *filedata = next_pos(filename, namesize);

        FSItem *result = initramfs_create(fs_root, filename, header, filedata);
        if (!result){
            error_color();
            print("[ERROR] cannot create file \"");
            print(filename);
            print("\"!");
            raise_err("");
        }
        
        // 下一个文件位置（对齐 4B）
        header = (CPIOHeader*)next_pos(filedata, filesize);
    }
}



