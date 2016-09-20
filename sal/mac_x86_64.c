
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

#if SYS_BYTE_ORDER == LE
void write_double_le(void *addr, double *d)
{
    *((double *)addr) = *d;
}

void write_double_be(void *addr, double *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[7]; a[1] = b[6]; a[2] = b[5]; a[3] = b[4];
    a[4] = b[3]; a[5] = b[2]; a[6] = b[1]; a[7] = b[0];
}

void write_uint16_le(void *addr, uint16_t *d)
{
    *((uint16_t *)addr) = *d;
}

void write_uint16_be(void *addr, uint16_t *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[1]; a[1] = b[0];
}

void write_uint32_le(void *addr, uint32_t *d)
{
    *((uint32_t *)addr) = *d;
}

void write_uint32_be(void *addr, uint32_t *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[3]; a[1] = b[2]; a[2] = b[1]; a[3] = b[0];
}

uint32_t read_uint32_le(void *addr)
{
    return *((uint32_t *)addr);
}

uint32_t read_uint32_be(void *addr)
{
    uint8_t *p = (uint8_t *)addr;

    return p[0] * 0x1000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
}

double read_double_le(void *addr)
{
    return *((double *)addr);
}

double read_double_be(void *addr)
{
    union {
        uint8_t u[8];
        double  d;
    } a;
    uint8_t *b = (uint8_t *)addr;

    a.u[0] = b[7]; a.u[1] = b[6]; a.u[2] = b[5]; a.u[3] = b[4];
    a.u[4] = b[3]; a.u[5] = b[2]; a.u[6] = b[1]; a.u[7] = b[0];

    return a.d;
}

#else
void write_double_be(void *addr, double *d)
{
    *((double *)addr) = *d;
}

void write_double_le(void *addr, double *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[7]; a[1] = b[6]; a[2] = b[5]; a[3] = b[4];
    a[4] = b[3]; a[5] = b[2]; a[6] = b[1]; a[7] = b[0];
}

void write_uint16_be(void *addr, uint16_t *d)
{
    *((uint16_t *)addr) = *d;
}

void write_uint16_le(void *addr, uint16_t *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[1]; a[1] = b[0];
}

void write_uint32_be(void *addr, uint32_t *d)
{
    *((uint32_t *)addr) = *d;
}

void write_uint32_le(void *addr, uint32_t *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[3]; a[1] = b[2]; a[2] = b[1]; a[3] = b[0];
}

uint32_t read_uint32_be(void *addr)
{
    return *((uint32_t *)addr);
}

uint32_t read_uint32_le(void *addr)
{
    uint8_t *p = (uint8_t *)addr;

    return p[0] * 0x1000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
}

double read_double_be(void *addr)
{
    return *((double *)addr);
}

double read_double_le(void *addr)
{
    union {
        uint8_t u[8];
        double  d;
    } a;
    uint8_t *b = (uint8_t *)addr;

    a.u[0] = b[7]; a.u[1] = b[6]; a.u[2] = b[5]; a.u[3] = b[4];
    a.u[4] = b[3]; a.u[5] = b[2]; a.u[6] = b[1]; a.u[7] = b[0];

    return a.d;
}

#endif

