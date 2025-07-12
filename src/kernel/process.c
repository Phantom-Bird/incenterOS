// #include <stdint.h>
// #include <elf.h>
// #include "../shared/addr.h"
// #include "paging.h"

// typedef struct Process {
//     uint32_t pid;           // 进程ID
//     uint64_t ppid;          // 父进程ID
//     uint64_t state;         // 进程状态
//     uint64_t priority;      // 调度优先级
//     uint64_t esp;           // 栈指针
//     uint64_t eip;           // 指令指针
//     PhysicalAddress cr3;    // 页目录地址
//     uint64_t user_stack;    // 用户栈地址
//     struct process *next;   // 下一个进程
// } Process;

// #define CANNOT_ALLOC_PAGE 1

// uint64_t init_process(Process *process){
//     process->cr3 = alloc_table();
//     if (!process->cr3) {
//         return 1;
//     }


// }

// void *load_elf(void *elf_buffer) {
    

//     Elf64_Ehdr *ehdr = (Elf64_Ehdr*) elf_buffer;

//     if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
//         ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
//         ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
//         ehdr->e_ident[EI_MAG3] != ELFMAG3){
//             Err(L"Error: Invalid ELF");
//     }

//     Elf64_Phdr *phdrs = (Elf64_Phdr*) (elf_buffer + ehdr->e_phoff);

//     for(int i=0; i<ehdr->e_phnum; i++){
//         Elf64_Phdr *ph = &phdrs[i];
//         if (ph->p_type != PT_LOAD){
//             continue;
//         }

//         void *src = elf_buffer + ph->p_offset;
//         void *dest = (void*)(uint64_t) ph->p_vaddr - HIGH_ADDR;

//         PhysicalAddress addr = (uint64_t)dest & ~PageMask;
//         uint64_t num_pages = (ph->p_memsz + PageSize-1) / PageSize;

//         TryAllocPagesAt(addr, num_pages);

//         memcpy(dest, src, ph->p_filesz);

//         if (ph->p_memsz > ph->p_filesz){
//             memset((uint8_t*)dest + ph->p_filesz, ph->p_memsz - ph->p_filesz, 0);
//         }
//     }

//     return (void*)(ehdr->e_entry);
// }
