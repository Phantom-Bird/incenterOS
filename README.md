# incenterOS

![](incenterOS.png)

## 环境

```sh
sudo apt install build-essential nasm qemu-system-x86 ovmf gnu-efi dosfstools mtools clang lld
mkdir -p inc
cp /usr/include/efi/* inc
cp /usr/include/elf.h inc
```

## 构建

```sh
make clean all run
```
