/*
 *
 */

#include "htbl.h"

#define HAVE_ZERO_MEMBER    (0x1)
#define HAVE_VACA_MEMBER    (0x2)
#define VACATED             (1)

typedef struct htbl_head_t {
    uint32_t size;
    uint32_t hold;
    uint32_t flags;
    uint32_t (*hash)(intptr_t);
    int (*compare)(intptr_t, intptr_t);
    intptr_t *tbl;
} htbl_head_t;

static inline uint32_t htbl_key(uint32_t size, uint32_t hash, int i) {
    return hash + i * (hash * 2 + 1);
}

static inline int htbl_over_threshold(uint32_t size, unsigned hold) {
    uint32_t left = size - hold;

    return hold > (left << 2);
}

static void htbl_extend(htbl_head_t *head)
{
    uint32_t size = head->size * 2;
    intptr_t *tbl = (intptr_t *)calloc(size, sizeof(intptr_t));

    if (tbl) {
        int i, j;

        //printf("\nHash data copy:\n");
        // data copy to new table buffer, from old
        for (j = 0; j < head->size; j++) {
            intptr_t data = head->tbl[j];
            uint32_t hash;

            if (data == 0 || data == VACATED) {
                continue;
            }

            hash = head->hash ? head->hash(data) : data;
            for (i = 0; i < size; i++) {
                uint32_t pos = htbl_key(size, hash, i) % size;

                if (tbl[pos] == 0) {
                    tbl[pos] = data;
                    //printf("%ld try times: %d, pos: %d\n", data, i, pos);
                    break;
                }
            }
        }

        free(head->tbl);

        head->tbl = tbl;
        head->size = size;
    }
}

intptr_t htbl_create(uint32_t (*hash)(intptr_t), int (*compare)(intptr_t, intptr_t))
{
    htbl_head_t *head = (htbl_head_t *)malloc(sizeof(htbl_head_t));

    if (head) {
        head->tbl = (intptr_t *)calloc(DEF_HTBL_SIZE, sizeof(intptr_t));

        if (head->tbl) {
            head->size = DEF_HTBL_SIZE;
            head->hold = 0;
            head->flags = 0;
            head->hash = hash;
            head->compare = compare;
        } else {
            free(head);
            head = NULL;
        }
    }

    return (intptr_t) head;
}

int htbl_destroy(intptr_t htbl, void (*destroy)(intptr_t))
{
    htbl_head_t *head = (htbl_head_t *)htbl;

    if (head) {
        if (destroy) {
            int i = 0;
            for (i = 0; i < head->size; i++) {
                if (head->tbl[i] != 0 || head->tbl[i] == VACATED) {
                    continue;
                }
                destroy(head->tbl[i]);
            }
            if (head->flags & HAVE_VACA_MEMBER) {
                destroy(VACATED);
            }
            if (head->flags & HAVE_ZERO_MEMBER) {
                destroy(0);
            }
        }

        free(head->tbl);
        free(head);
        return 0;
    }
    return -1;
}

int htbl_length(intptr_t htbl)
{
    htbl_head_t *head = (htbl_head_t *)htbl;

    if (head) {
        return head->hold;
    }

    return -1;
}

int htbl_insert(intptr_t htbl, intptr_t data)
{
    htbl_head_t *head = (htbl_head_t *)htbl;
    uint32_t size, pos, i, hash;
    intptr_t *tbl;

    if (!head) {
        return -1;
    }

    if (data == 0) {
        if (!(head->flags & HAVE_ZERO_MEMBER)) {
            head->flags |= HAVE_ZERO_MEMBER;
            head->hold++;
        }
        return 1;
    }

    if (data == VACATED) {
        if (!(head->flags & HAVE_VACA_MEMBER)) {
            head->flags |= HAVE_VACA_MEMBER;
            head->hold++;
        }
        return 1;
    }

    if (htbl_lookup(htbl, data, NULL) == 1) {
        return 1;
    }


    if (htbl_over_threshold(head->size, head->hold)) {
        htbl_extend(head);
    }

    size = head->size;
    hash = head->hash ? head->hash(data) : data;
    tbl  = head->tbl;

    for (i = 0; i < size; i++) {
        pos = htbl_key(size, hash, i) % size;

        if (tbl[pos] == 0 || tbl[pos] == VACATED) {
            tbl[pos] = data;
            head->hold++;
            return 1;
        }
    }

    return 0;
}

int htbl_remove(intptr_t htbl, intptr_t data)
{
    htbl_head_t *head = (htbl_head_t *)htbl;
    uint32_t size, pos, i, hash;
    int      (*cmp)(intptr_t, intptr_t);
    intptr_t *tbl;

    if (!head) {
        return -1;
    }

    if (head->hold == 0) {
        return 0;
    }

    if (data == 0) {
        if (head->flags & HAVE_ZERO_MEMBER) {
            head->flags ^= HAVE_ZERO_MEMBER;
            head->hold--;
            return 1;
        } else {
            return 0;
        }
    }

    if (data == VACATED) {
        if(head->flags & HAVE_VACA_MEMBER) {
            head->flags ^= HAVE_VACA_MEMBER;
            head->hold--;
            return 1;
        } else {
            return 0;
        }
    }

    size = head->size;
    hash = head->hash ? head->hash(data) : data;
    cmp  = head->compare;
    tbl  = head->tbl;

    if (cmp) {
        for (i = 0; i < size; i++) {
            pos = htbl_key(size, hash, i) % size;

            if (tbl[pos] == 0) {
                break;
            }
            if (!cmp(data, tbl[pos])) {
                tbl[pos] = VACATED;
                head->hold--;
                return 1;
            }
        }
    } else {
        for (i = 0; i < size; i++) {
            pos = htbl_key(size, hash, i) % size;

            if (tbl[pos] == 0) {
                break;
            }
            if (data == tbl[pos]) {
                tbl[pos] = VACATED;
                head->hold--;
                return 1;
            }
        }
    }

    return 0;
}

int htbl_lookup(intptr_t htbl, intptr_t data, intptr_t *res)
{
    htbl_head_t *head = (htbl_head_t *)htbl;
    uint32_t size, pos, i, hash;
    int      (*cmp)(intptr_t, intptr_t);
    intptr_t *tbl;

    if (!head) {
        return -1;
    }

    if (head->hold == 0) {
        return 0;
    }

    if (data == 0) {
        return (head->flags & HAVE_ZERO_MEMBER) ? 1 : 0;
    }

    if (data == VACATED) {
        return (head->flags & HAVE_VACA_MEMBER) ? 1 : 0;
    }

    size = head->size;
    hash = head->hash ? head->hash(data) : data;
    cmp  = head->compare;
    tbl  = head->tbl;

    if (cmp) {
        for (i = 0; i < size; i++) {
            pos = htbl_key(size, hash, i) % size;

            if (tbl[pos] == 0) {
                break;
            }
            if (!cmp(data, tbl[pos])) {
                if (res) *res = tbl[pos];
                return 1;
            }
        }
    } else {
        for (i = 0; i < size; i++) {
            pos = htbl_key(size, hash, i) % size;

            if (tbl[pos] == 0) {
                break;
            }
            if (data == tbl[pos]) {
                return 1;
            }
        }
    }

    return 0;
}

