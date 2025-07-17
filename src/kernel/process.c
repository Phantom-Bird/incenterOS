#include "process.h"
#include <elf.h>
#include "../shared/addr.h"
#include "process_paging.h"
#include "paging.h"
#include "pmm_alloc.h"

extern PhysicalAddress kernel_pml4_phys;
Process *process_list_head = NULL;
Process *process_list_tail = NULL;

uint64_t check_elf(void *elf_buffer);
uint64_t alloc_pages_for(void *elf_buffer, Process *process);
uint64_t load_elf(void *elf_buffer, int64_t *out_entry);
uint64_t map_high_addr(Process *process);
uint64_t load_stack(Process *process);
uint64_t setup_context(Process *process);
uint64_t add_to_list(Process *process);

#define STMT_WRAPPER(stmt) do {stmt} while(0)
#define CHECK(status) STMT_WRAPPER(uint64_t __status__=(status); if (__status__) return __status__;)

uint64_t current_pid = 1;

uint64_t new_process(void *elf_buffer, uint64_t ppid){
    Process *process = kmalloc(sizeof(Process));
    process->pid = current_pid++;
    process->ppid = ppid;

    process->cr3 = paging_get_root();
    if (!process->cr3) {
        return FAILED_TO_ALLOC_PAGE;
    }
    
    CHECK(check_elf(elf_buffer));
    CHECK(alloc_pages_for(elf_buffer, &process));
    CHECK(map_high_addr(&process));
    CHECK(load_elf(elf_buffer, &process->rip_init));
    CHECK(load_stack(process));
    CHECK(setup_context(process));
    CHECK(add_to_list(process));

    return OK;
}

uint64_t delete_process(Process *process){
    for (ProcessPage *page = process->pages, *next=NULL; page; page=next){
        if (!(page->flags & SHARED)){
            free_page(page->page);
        }

        next = page->next;
        kfree(page);
    }

    kfree(process);
    return OK;
}


void save_context(Process *process, SavedRegisters *regs){
    memcpy(&process->context, regs, sizeof(SavedRegisters));
}

void enter_process(Process *process){
    __asm__ volatile (
        "movq %[proc_cr3], %%cr3;"
        "movq %[proc_context], %%rsp;"  // 以借用 POP_ALL_REGISTERS
        :
        : [proc_cr3] "r"(process->cr3),
          [proc_context] "r"(&process->context)
    );
    __asm__ volatile (
        POP_ALL_REGISTERS
        "iretq;"
    );
}


uint64_t check_elf(void *elf_buffer){
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*) elf_buffer;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3){
            return INVALID_ELF;
    }

    return OK;
}

uint64_t alloc_pages_for(void *elf_buffer, Process *process) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*) elf_buffer;
    Elf64_Phdr *phdrs = (Elf64_Phdr*) (elf_buffer + ehdr->e_phoff);

    for(int i=0; i<ehdr->e_phnum; i++){
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD){
            continue;
        }

        PhysicalAddress addr = ph->p_vaddr;
        addr &= ~0xFFFull;
        uint64_t num_pages = (ph->p_memsz + PAGE_SIZE-1) / PAGE_SIZE;
        
        uint64_t flags = PRESENT | USER;
        if (ph->p_flags | PF_W){
            flags |= WRITABLE;
        }
        if (!(ph->p_flags & PF_X)){
            flags |= NO_EXEC;
        }

        for (uint64_t i=0; i<num_pages; i++){
            proc_map_page(process, addr + i * PAGE_SIZE, proc_alloc_page(process, 0), flags);
        }
    }

    return OK;
}

uint64_t load_elf(void *elf_buffer, int64_t *out_entry) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*) elf_buffer;
    Elf64_Phdr *phdrs = (Elf64_Phdr*) (elf_buffer + ehdr->e_phoff);

    for(int i=0; i<ehdr->e_phnum; i++){
        Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD){
            continue;
        }

        void *src = elf_buffer + ph->p_offset;
        void *dest = (void*)(uint64_t) ph->p_vaddr;

        memcpy(dest, src, ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz){
            memset((uint8_t*)dest + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
    }

    *out_entry = (ehdr->e_entry);
    return OK;
}

uint64_t map_high_addr(Process *process) {
    PageTable user_pml4 = (PageTable)phys2virt(process->cr3);
    PageTable kern_pml4 = (PageTable)phys2virt(kernel_pml4_phys);
    
    // 复制高地址页表
    user_pml4[511] = kern_pml4[511];
    user_pml4[511].value &= ~USER;  // 清除USER位
    
    return OK;
}
uint64_t load_stack(Process *process){
    for (uint64_t i=0; i<USER_STACK_PAGES; i++){
        PhysicalAddress phys = proc_alloc_page(process, STACK);

        if (!phys){
            return FAILED_TO_ALLOC_PAGE;
        }

        proc_map_page(process, USER_STACK_BASE + i * 0x1000, phys, PRESENT | WRITABLE);
    }

    process->rsp_init = USER_STACK_VIRT;

    return OK;
}

uint64_t setup_context(Process *process){
    memset(&process->context, 0, sizeof(SavedRegisters));

    process->context.rsp = process->rsp_init;
    process->context.rip = process->rip_init;
    process->context.rflags = 0x202;
    process->context.cs = 0x08;
    process->context.ss = 0x10;

    return OK;
}

uint64_t add_to_list(Process *process){
    if (!process_list_head || !process_list_tail){
        process_list_head = process_list_tail = process;
        return OK;
    }

    process->next = process_list_head;
    process_list_tail->next = process;
    process_list_tail = process;

    return OK;
}
