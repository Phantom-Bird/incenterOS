#include "loadkernel.h"


KERNEL_ENTRY LoadKernel(void *KernelBuffer) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*) KernelBuffer;

    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3){
            Err(L"Error: Invalid ELF");
    }

    Elf64_Phdr *phdrs = (Elf64_Phdr*) (KernelBuffer + ehdr->e_phoff);

    for(int i=0; i<ehdr->e_phnum; i++){
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD){
            continue;
        }

        void *src = KernelBuffer + ph->p_offset;
        void *dest = (void*)(UINTN) ph->p_vaddr;

        EFI_PHYSICAL_ADDRESS addr = ph->p_vaddr & ~PageMask;
        UINTN num_pages = (ph->p_memsz + PageSize-1) / PageSize;

        TryAllocPagesAt(addr, num_pages);

        BS->CopyMem(dest, src, ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz){
            BS->SetMem((uint8_t*)dest + ph->p_filesz, ph->p_memsz - ph->p_filesz, 0);
        }
    }

    return (KERNEL_ENTRY)(UINTN)(ehdr->e_entry);
}
