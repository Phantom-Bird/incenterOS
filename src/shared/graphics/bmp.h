#pragma once

#include <efi.h>

#pragma pack(push, 1)  // 取消对齐
typedef struct {
    UINT8 magic[2];       // 'B' 'M'
    UINT32 file_size;
    UINT16 reserved1;
    UINT16 reserved2;
    UINT32 pixel_offset;
} BMPHeader;

typedef struct {
    UINT32 header_size;
    INT32 width;
    INT32 height;
    UINT16 planes;
    UINT16 bit_count;
    UINT32 compression;
    UINT32 image_size;
    INT32 x_pixels_per_meter;
    INT32 y_pixels_per_meter;
    UINT32 colors_used;
    UINT32 colors_important;
} DIBHeader;

typedef struct {
    BMPHeader bmp;
    DIBHeader dib;
    CHAR8 _[];
} BMP;


#pragma pack(pop)
