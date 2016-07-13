#include "vect.h"

typedef struct vect_head_t {
    uint32_t size;
    uint32_t bgn;
    uint32_t end;
    intptr_t *ptr;
} vect_head_t;

static inline int vect_index_calc(int length, int i) {
    if (i < 0) {
        if (-i <= length) {
            i = length + i;
        } else {
            i = length + (i % length);
        }
    }
    return i < length ? i : length;
}

static inline vect_head_t *vect_head_alloc(uint32_t size) {
    vect_head_t *head = (vect_head_t *)malloc(sizeof(vect_head_t));

    if (head) {
        uint32_t cap = size < DEF_VECT_SIZE ? DEF_VECT_SIZE : size;
        intptr_t *ptr;

        cap = SIZE_ALIGN_16(cap);
        ptr = (intptr_t *)malloc(sizeof(intptr_t) * cap);
        if (!ptr) {
            free(head);
            return NULL;
        }
        head->size = cap;
        head->ptr = ptr;
        head->bgn = 0;
        head->end = size;
    }

    return head;
}

static inline void vect_head_free(vect_head_t *head) {
    free(head->ptr);
    free(head);
}

static inline void swap(intptr_t *buf, uint32_t a, uint32_t b) {
    intptr_t tmp = buf[a];

    buf[a] = buf[b];
    buf[b] = tmp;
}

static void max_heapify(intptr_t *buf, uint32_t start, uint32_t end)
{
    uint32_t dad = start;
    uint32_t son = dad * 2 + 1;

    while(son < end) {
        // choice the max
        if (son + 1 < end && buf[son] < buf[son + 1]) {
            son ++;
        }

        if (buf[son] > buf[dad]) {
            swap(buf, dad, son);
            dad = son;
            son = dad * 2 + 1;
        } else {
            return;
        }
    }
}

static int index_of(intptr_t *buf, uint32_t start, uint32_t end, intptr_t data)
{
    uint32_t pos = start;
    while(pos < end) {
        if (buf[pos] == data) {
            return pos;
        }
        pos++;
    }

    return -1;
}

static int compare_index_of(intptr_t *buf, uint32_t start, uint32_t end, intptr_t data,
                          int (*compare)(intptr_t, intptr_t))
{
    uint32_t pos = start;
    while(pos < end) {
        if (!compare(buf[pos],data)) {
            return pos;
        }
        pos++;
    }

    return -1;
}

static void cmp_heapify(intptr_t *buf, uint32_t start, uint32_t end, int (*compare)(intptr_t, intptr_t))
{
    uint32_t dad = start;
    uint32_t son = dad * 2 + 1;

    while(son < end) {
        // choice the max
        if (son + 1 < end && compare(buf[son], buf[son + 1]) < 0) {
            son ++;
        }

        if (compare(buf[son], buf[dad]) > 0) {
            swap(buf, dad, son);
            dad = son;
            son = dad * 2 + 1;
        } else {
            return;
        }
    }
}

intptr_t vect_create(uint32_t n, intptr_t *datas)
{
    vect_head_t *vect;

    vect = vect_head_alloc(n);
    if (vect && datas) {
        uint32_t i;
        for(i = 0; i < n; i++) {
            vect->ptr[i] = datas[i];
        }
    }

    return (intptr_t)vect;
}

int vect_length(intptr_t v)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head) {
        return -1;
    }

    return head->end - head->bgn;
}

int vect_destroy(intptr_t v)
{
    vect_head_t *head = (vect_head_t *)v;

    if (head) {
        vect_head_free(head);
        return 0;
    }

    return -1;
}

intptr_t *vect_ptr(intptr_t v)
{
    vect_head_t *head = (vect_head_t *)v;

    if (head) {
        return head->ptr + head->bgn;
    }
    return NULL;
}

int vect_set(intptr_t v, int i, intptr_t d)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head) {
        return -1;
    }

    if (head->bgn + i < head->end) {
        head->ptr[head->bgn + i] = d;
        return 1;
    }

    return 0;
}

int vect_index_of(intptr_t v, int start, intptr_t d, int (*compare)(intptr_t, intptr_t))
{
    vect_head_t *head = (vect_head_t *)v;
    intptr_t *buf;
    uint32_t end;

    if (!head) {
        return -1;
    }

    start = start < 0 ? 0 : start;
    buf = head->ptr + head->bgn;
    end = head->end - head->bgn;

    if (compare) {
        return compare_index_of(buf, start, end, d, compare);
    } else {
        return index_of(buf, start, end, d);
    }
}

int vect_get(intptr_t v, int i, intptr_t *d)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head || !d) {
        return -1;
    }

    if (head->bgn + i < head->end) {
        *d = head->ptr[head->bgn + i];
        return 1;
    }

    return 0;
}

int vect_sets(intptr_t v, int start, int n, intptr_t *data);
int vect_gets(intptr_t v, int start, int n, intptr_t *data);

int vect_push(intptr_t v, intptr_t data)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head) {
        return -1;
    }

    if (head->end == head->size) {
        intptr_t *ptr = (intptr_t*)malloc(head->size * 2 * sizeof(intptr_t));

        if (!ptr) {
            //TODO: try shift left
            return 0;
        }

        memcpy(ptr + head->bgn, head->ptr + head->bgn, (head->end - head->bgn) * sizeof(intptr_t));
        free(head->ptr);
        head->size *= 2;
        head->ptr = ptr;
    }

    head->ptr[head->end++] = data;

    return 1;
}

int vect_lpush(intptr_t v, intptr_t data)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head) {
        return -1;
    }

    if (head->bgn == 0) {
        uint32_t size, bgn;
        intptr_t *ptr;

        if (head->end == head->size) {
            size = head->size * 2;
        } else {
            size = head->size;
        }

        ptr = (intptr_t*)malloc(size * sizeof(intptr_t));
        if (!ptr) {
            //TODO: try shift right
            return 0;
        }
        bgn = head->bgn + size - head->end;

        memcpy(ptr + bgn, head->ptr + head->bgn, (size - bgn) * sizeof(intptr_t));

        free(head->ptr);
        head->ptr = ptr;
        head->bgn = bgn;
        head->end = size;
        head->size = size;
    }

    head->ptr[--head->bgn] = data;

    return 1;
}

int vect_pop(intptr_t v, intptr_t *data)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head || !data) {
        return -1;
    }

    if (head->bgn < head->end) {
        *data = head->ptr[--head->end];
        return 1;
    }
    return 0;
}

int vect_lpop(intptr_t v, intptr_t *data)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head || !data) {
        return -1;
    }

    if (head->bgn < head->end) {
        *data = head->ptr[head->bgn++];
        return 1;
    }
    return 0;
}

int vect_insert(intptr_t v, int start, int n, intptr_t *datas)
{
    vect_head_t *head = (vect_head_t *)v;
    uint32_t size;
    intptr_t *ptr;
    int length;

    if (!head) {
        return -1;
    }

    if (n < 1 || !datas) {
        return 0;
    }

    length = head->end - head->bgn;
    if (start > length) {
        return 0;
    }

    start = vect_index_calc(length, start);
    size = length + n;
    size = size < head->size ? head->size : size;
    ptr = (intptr_t*)malloc(size * sizeof(intptr_t));
    if (!ptr) {
        return -1;
    }

    if (start == 0) {
        memcpy(ptr, datas, n * sizeof(intptr_t));
        memcpy(ptr + n, head->ptr + head->bgn, length * sizeof(intptr_t));
    } else if (start == length){
        memcpy(ptr, head->ptr + head->bgn, length * sizeof(intptr_t));
        memcpy(ptr + length, datas, n * sizeof(intptr_t));
    } else {
        memcpy(ptr, head->ptr + head->bgn, start * sizeof(intptr_t));
        memcpy(ptr + start, datas, n * sizeof(intptr_t));
        memcpy(ptr + (start + n), head->ptr + head->bgn + start, (length - start) * sizeof(intptr_t));
    }

    free(head->ptr);
    head->ptr = ptr;
    head->size = size;
    head->end = length + n;
    head->bgn = 0;

    return n;
}

intptr_t vect_slice(intptr_t v, int start, int end)
{
    vect_head_t *head = (vect_head_t *)v;
    int length;

    if (!head) {
        return 0;
    }

    length = head->end - head->bgn;
    if (length == 0) {
        start = end = 0;
    } else {
        start = vect_index_calc(length, start);
        if (start < length) {
            end = vect_index_calc(length, end);
            end = end > start ? end: start;
        } else {
            end = length;
        }
    }

    return vect_create(end - start, vect_ptr(v) + start);
}

intptr_t vect_cut(intptr_t v, int start, int end)
{
    vect_head_t *head = (vect_head_t *)v;
    vect_head_t *head2;
    int length;

    if (!head) {
        return 0;
    }

    length = head->end - head->bgn;
    if (length == 0) {
        start = end = 0;
    } else {
        start = vect_index_calc(length, start);
        if (start < length) {
            end = vect_index_calc(length, end);
            end = end > start ? end: start;
        } else {
            end = length;
        }
    }

    if (start == end) {
        return vect_create(0, NULL);
    }

    head2 = vect_head_alloc(length - (end - start));
    if (head2) {
        uint32_t size = head2->size;
        intptr_t *ptr = head2->ptr;

        if (start != 0 && end != length) {
            memcpy(ptr, head->ptr + head->bgn, start * sizeof(intptr_t));
            memcpy(ptr + start, head->ptr + head->bgn + end, (length - end) * sizeof(intptr_t));
        } else if (start != 0) {
            memcpy(ptr, head->ptr + head->bgn, start * sizeof(intptr_t));
        } else if (end != length) {
            memcpy(ptr, head->ptr + head->bgn + end, (length - end) * sizeof(intptr_t));
        }

        // swap ptr of vect & vect2
        head2->size = head->size;
        head2->ptr = head->ptr;
        head2->bgn = head->bgn + start;
        head2->end = head->end - (length - end);

        head->size = size;
        head->ptr = ptr;
        head->bgn = 0;
        head->end = length - (end - start);
    }

    return (intptr_t)head2;
}

intptr_t vect_splice(intptr_t v, int start, int end, int n, intptr_t *datas)
{
    vect_head_t *head = (vect_head_t *)v;
    vect_head_t *head2;
    int length;

    if (!head) {
        return 0;
    }

    n = n < 0 ? 0 : n;
    if (n && datas == NULL) {
        return 0;
    }

    length = head->end - head->bgn;
    if (length == 0) {
        start = end = 0;
    } else {
        start = vect_index_calc(length, start);
        if (start < length) {
            end = vect_index_calc(length, end);
            end = end > start ? end: start;
        } else {
            end = length;
        }
    }

    if (start >= end) {
        return vect_create(0, NULL);
    }

    head2 = vect_head_alloc(n + length - (end - start));
    if (head2) {
        uint32_t size = head2->size;
        intptr_t *ptr = head2->ptr;

        if (start != 0 && end != length) {
            memcpy(ptr, head->ptr + head->bgn, start * sizeof(intptr_t));
            memcpy(ptr + start, datas, n * sizeof(intptr_t));
            memcpy(ptr + start + n, head->ptr + head->bgn + end, (length - end) * sizeof(intptr_t));
        } else if (start != 0) { memcpy(ptr, head->ptr + head->bgn, start * sizeof(intptr_t));
            memcpy(ptr + start, datas, n * sizeof(intptr_t));
        } else if (end != length) {
            memcpy(ptr, datas, n * sizeof(intptr_t));
            memcpy(ptr + n, head->ptr + head->bgn + end, (length - end) * sizeof(intptr_t));
        }

        // swap ptr of vect & vect2
        head2->size = head->size;
        head2->ptr = head->ptr;
        head2->bgn = head->bgn + start;
        head2->end = head->end - (length - end);

        head->size = size;
        head->ptr = ptr;
        head->bgn = 0;
        head->end = length + n - (end - start);
    }

    return (intptr_t)head2;
}

/*
void vect_dump(intptr_t v, const char *s)
{
    vect_head_t *head = (vect_head_t *)v;
    int i, max = head->end - head->bgn;

    printf("vect list: %s\n", s);

    for (i = 0; i < max; i++) {
        printf("[%d]: %ld\n", i, head->ptr[head->bgn + i]);
    }
}
*/

intptr_t vect_concat(intptr_t a, intptr_t b)
{
    vect_head_t *heada = (vect_head_t *)a;
    vect_head_t *headb = (vect_head_t *)b;
    vect_head_t *head;
    uint32_t alen, blen;
    intptr_t *aptr, *bptr, *ptr;

    if (!a || !b) {
        return 0;
    }
    alen = heada->end - heada->bgn;
    blen = headb->end - headb->bgn;
    aptr = heada->ptr;
    bptr = headb->ptr;

    head = vect_head_alloc(alen + blen);
    if (head) {
        ptr = head->ptr;

        memcpy(ptr, aptr, alen * sizeof(intptr_t));
        memcpy(ptr + alen, bptr, blen * sizeof(intptr_t));
    }

    return (intptr_t) head;
}

int vect_foreach(intptr_t v, void (*fn)(intptr_t data, void *udata), void *udata)
{
    vect_head_t *head = (vect_head_t *)v;

    if (!head || !fn) {
        return -1;
    }

    if (head->bgn < head->end) {
        uint32_t max = head->end - head->bgn;
        intptr_t *ptr = head->ptr;
        uint32_t i;

        for (i = 0; i < max; i++) {
            fn(ptr[i], udata);
        }
    }

    return 0;
}

// heapify interface
int vect_heapify(intptr_t v, int (*compare)(intptr_t, intptr_t))
{
    vect_head_t *head = (vect_head_t *)v;
    intptr_t *buf;
    int num, i;

    if (!head) {
        return -1;
    }

    buf = head->ptr + head->bgn;
    num = head->end - head->bgn;

    if (compare) {
        for (i = num / 2 - 1; i >= 0; i--) {
            cmp_heapify(buf, i, num, compare);
        }
    } else {
        for (i = num / 2 - 1; i >= 0; i--) {
            max_heapify(buf, i, num);
        }
    }

    return 0;
}

int vect_heapify_extract(intptr_t v, intptr_t *d, int (*compare)(intptr_t, intptr_t))
{
    vect_head_t *head = (vect_head_t *)v;
    intptr_t *buf;
    int num, i;

    if (!head) {
        return -1;
    }

    buf = head->ptr + head->bgn;
    num = head->end - head->bgn;

    if (num < 1) {
        return 0;
    }

    *d = buf[0];
    head->end--;

    if (num > 1) {
        buf[0] = buf[--num];
    }

    if (num > 1) {
        if (compare) {
            for (i = num / 2 - 1; i >= 0; i--) {
                cmp_heapify(buf, i, num, compare);
            }
        } else {
            for (i = num / 2 - 1; i >= 0; i--) {
                max_heapify(buf, i, num);
            }
        }
    }

    return 1;
}

int vect_heapify_insert(intptr_t v, intptr_t data, int (*compare)(intptr_t , intptr_t))
{
    int ret = vect_push(v, data);

    if (ret == 1) {
        vect_head_t *head = (vect_head_t *)v;
        intptr_t *buf = head->ptr + head->bgn;
        uint32_t son = head->end - head->bgn - 1;

        if (compare) {
            while (son > 0) {
                uint32_t dad = (son - 1) / 2;

                if (compare(buf[son],buf[dad]) > 0) {
                    swap(buf, dad, son);
                    son = dad;
                } else {
                    break;
                }
            }
        } else {
            while (son > 0) {
                uint32_t dad = (son - 1) / 2;

                if (buf[son] > buf[dad]) {
                    swap(buf, dad, son);
                    son = dad;
                } else {
                    break;
                }
            }
        }
    }

    return ret;
}

// others
int vect_sort(intptr_t v, int (*compare)(intptr_t, intptr_t))
{
    vect_head_t *head = (vect_head_t *)v;
    intptr_t *buf;
    int num, i;

    if (!head) {
        return -1;
    }

    buf = head->ptr + head->bgn;
    num = head->end - head->bgn;

    if (compare) {
        for (i = num / 2 - 1; i >= 0; i--) {
            cmp_heapify(buf, i, num, compare);
        }
        for (i = num - 1; i > 0; i--) {
            swap(buf, 0, i);
            cmp_heapify(buf, 0, i, compare);
        }
    } else {
        for (i = num / 2 - 1; i >= 0; i--) {
            max_heapify(buf, i, num);
        }
        for (i = num - 1; i > 0; i--) {
            swap(buf, 0, i);
            max_heapify(buf, 0, i);
        }
    }

    return 0;
}

int vect_reverse(intptr_t v)
{
    vect_head_t *head = (vect_head_t *)v;
    intptr_t *buf;
    int h, t; // head, tail

    if (!head) {
        return -1;
    }

    buf = head->ptr + head->bgn;

    h = 0;
    t = head->end - head->bgn - 1;
    while (h < t) {
        swap(buf, h++, t--);
    }

    return 0;
}

