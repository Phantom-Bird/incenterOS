#pragma once

#include <stdint.h>

typedef struct {
    // 所有数字都以十六进制字符串存储
    char magic[6];    // 必须为"070701"
    char ino[8];      // inode编号（16进制ASCII）
    char mode[8];     // 文件模式
    char uid[8];      // 用户ID
    char gid[8];      // 组ID
    char nlink[8];    // 硬链接数
    char mtime[8];    // 修改时间
    char filesize[8]; // 文件大小
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8]; // 文件名长度（含结尾\0）
    char check[8];    // 校验和（通常忽略）
} CPIOHeader;

#define MODE_DIR  0x4000  // mode 中目录的标志位
#define MODE_FILE  0x8000  // mode 中普通文件的标志位

void cpio_init(void *archive, uint64_t cpio_size);