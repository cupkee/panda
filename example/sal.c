
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int output(const char *s)
{
    return printf("%s", s);
}

void *file_load(const char *name, int *size)
{

    char *addr;
    int fd;
    struct stat sb;
    size_t length;

    fd = open(name, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    if (fstat(fd, &sb) == -1) {
        close(fd);
        return NULL;
    }
    length = sb.st_size + 1;

    addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    *size = length;

    return addr;
}

int file_release(void *addr, int size)
{
    return munmap(addr, size);
}

int file_store(const char *name, void *buf, int sz)
{
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC);
    int off, n;

    if (fd < 0) {
        return -1;
    }

    off = 0;
    while(off < sz) {
        n = write(fd, buf + off, sz - off);
        if (n < 0) {
            unlink(name);
            close(fd);
            return -1;
        }
        off += n;
    }

    close(fd);
    return 0;
}

int file_base_name(const char *name, void *buf, int sz)
{
    const char *base = rindex(name, '/');
    const char *suffix = rindex(name, '.');
    int len;

    base = base ? base : name;
    if (!suffix || suffix < base) {
        len = strlen(base);
    } else {
        len = suffix - base;
    }

    if (len < sz) {
        ((char *)buf)[len] = 0;
        memcpy(buf, base, len);
        return len;
    }

    return -1;
}

