
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

#define SIZE_ALIGN                  SIZE_ALIGN_64

// esbl profile
#define DEF_VECT_SIZE           (16)
#define DEF_HTBL_SIZE           (16)


// lang profile
#define EVAL_MAIN_VAR_MAX           (32)

#define DEF_STRING_SIZE             (8)

// lang compile resource default and limit
#define DEF_FUNC_SIZE               (4)
#define LIMIT_FUNC_SIZE             (512)

#define DEF_VMAP_SIZE               (8)
#define LIMIT_VMAP_SIZE             (255)

#define DEF_FUNC_CODE_SIZE          (32)
#define LIMIT_FUNC_CODE_SIZE        (32767)

#endif /* __CUPKEE_CONFIG__ */

