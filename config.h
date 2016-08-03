
#ifndef __CUPKEE_CONFIG__
#define __CUPKEE_CONFIG__

#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NULL
# define NULL ((void *)0)
#endif

#define CHAR_T char

#define SIZE_ALIGN_8(x)     (((x) + 7)  & ~0x07)
#define SIZE_ALIGN_16(x)    (((x) + 15) & ~0x0F)
#define SIZE_ALIGN_32(x)    (((x) + 31) & ~0x1F)
#define SIZE_ALIGN_64(x)    (((x) + 63) & ~0x3F)
#define SIZE_ALIGN_64(x)    (((x) + 63) & ~0x3F)

#define SIZE_ALIGN    SIZE_ALIGN_64

// esbl profile
#define DEF_VECT_SIZE       (16)
#define DEF_HTBL_SIZE       (16)


// lang profile
#define LEX_LINE_BUF_SIZE       (128)
#define LEX_TOK_SIZE            (32)

// lang compile resource default and limit
#define DEF_STATIC_NUM_SIZE     (32)
#define DEF_STATIC_STR_SIZE     (32)
#define DEF_NATIVE_FUNC_SIZE    (16)
#define DEF_FUNC_SIZE           (4)
#define DEF_VMAP_SIZE           (8)
#define DEF_FUNC_CODE_SIZE      (32)

#define LIMIT_FUNC_SIZE         (512)
#define LIMIT_NATIVE_FUNC_SIZE    (16)
#define LIMIT_VMAP_SIZE         (255)
#define LIMIT_FUNC_CODE_SIZE    (32767)
#define LIMIT_STATIC_NUM_SIZE   (1024)
#define LIMIT_STATIC_STR_SIZE   (1024)

#endif /* __CUPKEE_CONFIG__ */

