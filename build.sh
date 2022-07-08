gcc main.c                             \
      -c                                 \
      -fno-stack-protector               \
      -fpic                              \
      -fshort-wchar                      \
      -mno-red-zone                      \
      -I/usr/include/gnu-efi/inc	\
      -I /usr/include/gnu-efi/headers        \
      -I /usr/include/gnu-efi/headers/x86_64 \
      -DEFI_FUNCTION_WRAPPER             \
      -o main.o

ld main.o                         \
     /usr/include/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o     \
     -nostdlib                      \
     -znocombreloc                  \
     -T /usr/include/gnu-efi/gnuefi/elf_x86_64_efi.lds \
     -shared                        \
     -Bsymbolic                     \
     -L /usr/lib               \
     -l:libgnuefi.a                 \
     -l:libefi.a                    \
     -o main.so
     
 objcopy -j .text                \
          -j .sdata               \
          -j .data                \
          -j .dynamic             \
          -j .dynsym              \
          -j .rel                 \
          -j .rela                \
          -j .reloc               \
          --target=efi-app-x86_64 \
          main.so                 \
          BOOTX64.efi
          
rm main.o
rm main.so
cp BOOTX64.efi image/efi/boot/BOOTX64.efi
rm BOOTX64.efi
