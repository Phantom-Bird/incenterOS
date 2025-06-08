#include <efi.h>
#include <efilib.h>
#include "err.h"
#include "bmp.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL PIXEL;

void InitGOP();

void DrawPixel(UINT32 x, UINT32 y, PIXEL color);

void DrawRect(UINT32 L, UINT32 U, UINT32 R, UINT32 D, PIXEL color);

void DrawTextImage(CHAR8 *Image, PIXEL *ColorMap, UINT32 Unit, 
                   UINT32 StartX, UINT32 StartY);

// EFI_STATUS DrawBMP(BMP* bmp_data, UINT32 start_x, UINT32 start_y);
// 暂时不用
