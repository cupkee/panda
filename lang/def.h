/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#ifndef __LANG_DEF__
#define __LANG_DEF__

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/param.h>

#include <stdio.h>
#include <stdlib.h>
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
#define TOKEN_MAX_SIZE              (128)

# define INTERACTIVE_VAR_MAX        (32)

# define DEF_PROP_SIZE              (4)
# define DEF_ELEM_SIZE              (8)
# define DEF_FUNC_SIZE              (4)
# define DEF_VMAP_SIZE              (4)
# define DEF_FUNC_CODE_SIZE         (32)

# define LIMIT_VMAP_SIZE            (32)    // max variable number in function
# define LIMIT_FUNC_SIZE            (32767) // max function number in  module
# define LIMIT_FUNC_CODE_SIZE       (32767) // max code of each function

# define DEF_STRING_SIZE            (8)

// lang compile resource default and limit

#endif /* __LANG_DEF__ */

