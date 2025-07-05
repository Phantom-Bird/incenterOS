#include "gop.h"

// Failed because type (void**)GOP instead of (void**)&GOP
// But why status = 0x800...002?
// Undefined behavior???
// 这个故事告诉我们抄代码一定要抄对
// 花了我半天。

#ifdef BOOTLOADER
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
#endif
UINT32 ScreenWidth=-1, ScreenHeight=-1, Pitch;
UINT32 *fb;
UINT32 FBSize;

#ifdef BOOTLOADER
void InitGOP(){
    EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_STATUS status = BS->LocateProtocol(
        &GopGuid, NULL, (void**)&GOP
    );

    if (EFI_ERROR(status)){
        // ErrStatus(L"[ERROR] Failed to locate GOP\r\n", status);
        Err(L"[ERROR] Failed to locate GOP\r\n");
    }

    ScreenWidth = GOP->Mode->Info->HorizontalResolution;
    ScreenHeight = GOP->Mode->Info->VerticalResolution;
    Pitch = GOP->Mode->Info->PixelsPerScanLine;
    fb = (UINT32*) GOP->Mode->FrameBufferBase;
    FBSize = GOP->Mode->FrameBufferSize;
}
#endif

void InitGOPFrom(GraphicsInfo gi){
    fb = gi.framebuffer;
    ScreenWidth = gi.width;
    ScreenHeight = gi.height;
    FBSize = gi.size_bytes;
    Pitch = gi.pitch;
}

void DrawPixel(UINT32 x, UINT32 y, PIXEL color){
    if (x<0 || y<0 || x>=ScreenWidth || y>=ScreenHeight){
        return;
    }

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pixel = &fb[y * Pitch + x];
    pixel->Blue = color.Blue;
    pixel->Green = color.Green;
    pixel->Red = color.Red;
    pixel->Reserved = color.Reserved;
}

void DrawRect(UINT32 x1, UINT32 y1, UINT32 x2, UINT32 y2, PIXEL color){
    for (UINT32 x=x1; x<=x2; x++){
        for (UINT32 y=y1; y<=y2; y++){
            DrawPixel(x, y, color);
        }
    }
}

void DrawTextImage(CHAR8 *Image, PIXEL *ColorMap, UINT32 Unit, 
                   UINT32 StartX, UINT32 StartY){
    // ' ' 固定为透明
    int x=StartX, y=StartY;
    for (int i=0; Image[i]; i++){
        if (Image[i] == '\n'){
            x = StartX;
            y += Unit;
            continue;
        }
        
        if (Image[i] != ' '){
            DrawRect(x, y, x+Unit, y+Unit, ColorMap[Image[i]]);
        }
        x += Unit;
    }
}

void ClearScreen(){
    DrawRect(0, 0, ScreenWidth-1, ScreenHeight-1, (PIXEL){0, 0, 0, 255});
}

// EFI_STATUS DrawBMP(BMP* bmp_data, UINT32 start_x, UINT32 start_y) {
//     BMPHeader* bmp = &bmp_data->bmp;
//     DIBHeader* dib = &bmp_data->dib;

//     if (bmp->magic[0] != 'B' || bmp->magic[1] != 'M' || dib->bit_count != 24){
//         return EFI_UNSUPPORTED;
//     }

//     INT32 width = dib->width;
//     INT32 height = dib->height;
//     UINT8* pixel_data = bmp_data + bmp->pixel_offset;

//     UINTN line_size = ((width * 3 + 3) & ~3); // 每行对齐到 4 字节

//     for (INT32 y = 0; y < height; y++) {
//         for (INT32 x = 0; x < width; x++) {
//             UINT8* px = pixel_data + (height - 1 - y) * line_size + x * 3;
//             PIXEL color = {px[0], px[1], px[2], 0};
//             DrawPixel(start_x + x, start_y + y, color);
//         }
//     }

//     return EFI_SUCCESS;
// }

