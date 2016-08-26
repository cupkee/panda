
#ifndef __CUPKEE_CONFIG__
#define __CUPKEE_CONFIG__

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NULL
# define NULL ((void *)0)
#endif

#define LE                          1
#define BE                          2

#define ADDRSIZE_32                 1
#define ADDRSIZE_64                 2

#define SYS_BYTE_ORDER              (BYTE_ORDER == LITTLE_ENDIAN ? LE : BE)
#define SYS_ADDR_SIZE               ADDRSIZE_64

#define ADDR_ALIGN_4(a)             ((void *)((((intptr_t)(a)) +  3) & ~0x03L))
#define ADDR_ALIGN_8(a)             ((void *)((((intptr_t)(a)) +  7) & ~0x07L))
#define ADDR_ALIGN_16(a)            ((void *)((((intptr_t)(a)) + 15) & ~0x0FL))
#define ADDR_ALIGN_32(a)            ((void *)((((intptr_t)(a)) + 31) & ~0x1FL))

#define SIZE_ALIGN_4(x)             (((x) +  3) & ~0x03)
#define SIZE_ALIGN_8(x)             (((x) +  7) & ~0x07)
#define SIZE_ALIGN_16(x)            (((x) + 15) & ~0x0F)
#define SIZE_ALIGN_32(x)            (((x) + 31) & ~0x1F)
#define SIZE_ALIGN_64(x)            (((x) + 63) & ~0x3F)
#define SIZE_ALIGN_64(x)            (((x) + 63) & ~0x3F)

#define SIZE_ALIGN                  SIZE_ALIGN_8
#define ADDR_ALING                  ADDR_ALIGN_8

// lang profile
#define MAGIC_BASE                  (0xE0)

#define TOKEN_MAX_SIZE              (32)

//#define EXECUTABLE_MINI
#define EXECUTABLE_SMALL
//#define EXECUTABLE_NORMAL
//#define EXECUTABLE_LARGE
//#define EXECUTABLE_HUGE

#if defined(EXECUTABLE_MINI)

# define INTERACTIVE_VAR_MAX        (16)

# define EXE_FUNCTION_MAX           (16)
# define EXE_NUMBER_MAX             (32)
# define EXE_STRING_MAX             (128)
# define EXE_NATIVE_MAX             (16)
# define EXE_MAIN_CODE_MAX          (512)
# define EXE_FUNC_CODE_MAX          (1536)

# define DEF_FUNC_SIZE              (4)
# define DEF_VMAP_SIZE              (4)
# define DEF_FUNC_CODE_SIZE         (32)
# define LIMIT_VMAP_SIZE            (16)
# define LIMIT_FUNC_SIZE            (16)
# define LIMIT_FUNC_CODE_SIZE       (32767)

# define DEF_STRING_SIZE            (8)

#elif defined(EXECUTABLE_SMALL)

# define INTERACTIVE_VAR_MAX        (32)

# define EXE_FUNCTION_MAX           (32)
# define EXE_NUMBER_MAX             (64)
# define EXE_STRING_MAX             (256)
# define EXE_NATIVE_MAX             (32)
# define EXE_MAIN_CODE_MAX          (1024)
# define EXE_FUNC_CODE_MAX          (1024 * 3)

# define DEF_FUNC_SIZE              (4)
# define DEF_VMAP_SIZE              (4)
# define DEF_FUNC_CODE_SIZE         (32)
# define LIMIT_VMAP_SIZE            (32)
# define LIMIT_FUNC_SIZE            (32)
# define LIMIT_FUNC_CODE_SIZE       (32767)

# define DEF_STRING_SIZE            (8)
#else // NORMAL
#endif

#define EXE_MEM_SPACE               SIZE_ALIGN(EXE_FUNCTION_MAX * sizeof(void *) + EXE_NUMBER_MAX * sizeof(double)\
                                    + EXE_STRING_MAX * sizeof(void *) + EXE_NATIVE_MAX * 2 * sizeof(void *)\
                                    + EXE_MAIN_CODE_MAX + EXE_FUNC_CODE_MAX + 32)
#define SYMBAL_MEM_SPACE            SIZE_ALIGN(EXE_STRING_MAX * (sizeof(void *) + DEF_STRING_SIZE))

// lang compile resource default and limit

#endif /* __CUPKEE_CONFIG__ */

