#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <elf.h>

#define ERROR_MSG "Error: Not an ELF file\n"

void print_error() {
    write(STDERR_FILENO, ERROR_MSG, strlen(ERROR_MSG));
    exit(98);
}

void display_elf_header(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        print_error();
    }

    Elf64_Ehdr header;
    ssize_t bytes_read = read(fd, &header, sizeof(header));
    if (bytes_read == -1 || bytes_read != sizeof(header)) {
        print_error();
    }

    // Check if the file is an ELF file
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0) {
        print_error();
    }

    printf("Magic: ");
    for (int i = 0; i < SELFMAG; i++) {
        printf("%02x ", header.e_ident[i]);
    }
    printf("\n");

    printf("Class: %s\n", (header.e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32" : "ELF64");

    printf("Data: %s\n", (header.e_ident[EI_DATA] == ELFDATA2LSB) ? "2's complement, little endian" :
                        ((header.e_ident[EI_DATA] == ELFDATA2MSB) ? "2's complement, big endian" : "Unknown"));

    printf("Version: %d\n", header.e_ident[EI_VERSION]);

    printf("OS/ABI: ");
    switch (header.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV:
            printf("UNIX - System V\n");
            break;
        case ELFOSABI_LINUX:
            printf("UNIX - Linux\n");
            break;
        default:
            printf("Unknown\n");
    }

    printf("ABI Version: %d\n", header.e_ident[EI_ABIVERSION]);

    printf("Type: ");
    switch (header.e_type) {
        case ET_NONE:
            printf("NONE (No file type)\n");
            break;
        case ET_REL:
            printf("REL (Relocatable file)\n");
            break;
        case ET_EXEC:
            printf("EXEC (Executable file)\n");
            break;
        case ET_DYN:
            printf("DYN (Shared object file)\n");
            break;
        case ET_CORE:
            printf("CORE (Core file)\n");
            break;
        default:
            printf("Unknown\n");
    }

    printf("Entry point address: 0x%lx\n", header.e_entry);

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s elf_filename\n", argv[0]);
        return 1;
    }

    display_elf_header(argv[1]);

    return 0;
}
