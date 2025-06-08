# === Toolchain ===
CLANG      := clang
LD         := ld.lld
LLD_LINK   := lld-link
OBJCOPY    := llvm-objcopy

# === Directories ===
SRC_DIR     := src
INC_DIR     := inc
BOOT_DIR	:= $(SRC_DIR)/bootloader
KERNEL_DIR  := $(SRC_DIR)/kernel
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj

# === Flags ===
CFLAGS      := -I$(INC_DIR) -target x86_64-pc-win32-coff -ffreestanding -fno-stack-protector -mno-red-zone -Wall -Wextra
LDFLAGS_EFI := /subsystem:efi_application /entry:efi_main /out:$(BUILD_DIR)/BOOTX64.EFI
CFLAGS_KERN := -I$(INC_DIR) -ffreestanding -m64 -mno-red-zone -Wall -Wextra
LDFLAGS_KERN:= -nostdlib -z noexecstack -T$(KERNEL_DIR)/linker.ld

# === Files ===
BOOT_SRCS   := $(wildcard $(BOOT_DIR)/*.c)
BOOT_OBJS   := $(patsubst $(BOOT_DIR)/%.c, $(OBJ_DIR)/%.obj, $(BOOT_SRCS))
KERNEL_SRC  := $(KERNEL_DIR)/kernel.c
KERNEL_OBJ  := $(BUILD_DIR)/kernel.o
KERNEL_ELF  := $(BUILD_DIR)/kernel.elf
EFI_FILE    := $(BUILD_DIR)/BOOTX64.EFI
ESP_IMG     := $(BUILD_DIR)/esp.img

# === OVMF ===
OVMF_FLAGS	:= -bios OVMF.fd

# === Default Target ===
all: $(ESP_IMG)

# === Compile Bootloader .c → .obj ===
$(OBJ_DIR)/%.obj: $(BOOT_DIR)/%.c | $(OBJ_DIR)
	$(CLANG) $(CFLAGS) -c $< -o $@

# === Link .obj → BOOTX64.EFI ===
$(EFI_FILE): $(BOOT_OBJS)
	$(LLD_LINK) $(LDFLAGS_EFI) $(BOOT_OBJS)

# === Build Kernel ===
$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CLANG) $(CFLAGS_KERN) -c $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJ)
	$(LD) $(LDFLAGS_KERN) -o $@ $^

# === Create ESP Image ===
$(ESP_IMG): $(EFI_FILE) $(KERNEL_ELF)
	dd if=/dev/zero of=$@ bs=1M count=64
	mkfs.vfat $@
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(EFI_FILE) ::/EFI/BOOT/
	mcopy -i $@ $(KERNEL_ELF) ::/

# === Run QEMU ===
run: $(ESP_IMG)
	qemu-system-x86_64 $(OVMF_FLAGS) -drive format=raw,file=$(ESP_IMG)

# === Directory setup ===
$(OBJ_DIR) $(BUILD_DIR):
	mkdir -p $@

# === Clean ===
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
