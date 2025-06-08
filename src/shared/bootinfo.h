#include <stdint.h>

typedef struct {
    uint32_t *framebuffer;
    uint32_t width;
    uint32_t height;
} GraphicsInfo;


typedef struct {
    uint64_t magic = 0x46697265666c7900;

} BootInfo;