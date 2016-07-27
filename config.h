
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

// esbl profile
#define DEF_VECT_SIZE       (16)
#define DEF_HTBL_SIZE       (16)

#define SIZE_ALIGN_16(x)    (((x) + 15) & 0xfffffff0u)
#define SIZE_ALIGN_32(x)    (((x) + 31) & 0xffffffe0u)
#define SIZE_ALIGN_64(x)    (((x) + 63) & 0xffffffc0u)

// lang profile
#define LEX_LINE_BUF_SIZE   (128)
#define LEX_TOK_SIZE        (32)

#define DEF_FUNC_SIZE       (4)
#define DEF_VMAP_SIZE       (8)
#define DEF_CODE_SIZE       (32)
#define DEF_STATIC_NUM_SIZE (32)
#define STATIC_NUM_LIMIT    (65535)
#if STATIC_NUM_LIMIT > 255
#define NUM_CODE_2
#endif

#endif /* __CUPKEE_CONFIG__ */

