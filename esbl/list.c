#include "list.h"

typedef struct list_node_t {
    struct list_node_t *next;
    struct list_node_t *prev;
    intptr_t data;
} list_node_t;

typedef struct list_head_t {
    int flags;
    int length;
    struct list_node_t *head;
    struct list_node_t *tail;
} list_head_t;

typedef struct list_clip_t {
    struct list_node_t *start;
    int max;
} list_clip_t;

static inline list_head_t * list_head_alloc(int flags) {
    list_head_t *list = malloc(sizeof(list_head_t));

    if (list) {
        list->flags = flags;
        list->length = 0;
        list->head = NULL;
        list->tail = NULL;
    }

    return list;
}

static inline list_node_t *list_node_alloc(intptr_t data) {
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));

    if (node) {
        node->prev = NULL;
        node->next = NULL;
        node->data = data;
    }

    return node;
}

static inline int list_node_index_calc(int length, int i) {
    if (i < 0) {
        if (i >= -length) {
            i = length + i;
        } else {
            i = length + (i % length);
        }
    }

    return i < length ? i : length;
}

static inline list_node_t *list_node_get(list_head_t *list, int i) {
    list_node_t *node = NULL;

    if (i < 0) {
        if (i >= -list->length) {
            i = list->length + i;
        } else {
            i = list->length + (i % list->length);
        }
    }

    if (i < list->length / 2) {
        node = list->head;
        while(node && i--) {
            node = node->next;
        }
    } else
    if (i < list->length) {
        node = list->tail;
        i = list->length - i - 1;

        while(node && i--) {
            node = node->prev;
        }
    }

    return node;
}

static inline list_clip_t list_node_clip(list_head_t *list, int s, int e) {
    list_clip_t clip = {NULL, 0};

    s = list_node_index_calc(list->length, s);
    e = list_node_index_calc(list->length, e);

    if (s < list->length && s < e) {
        e = list->length < e ? list->length: e;

        clip.start = list_node_get(list, s);
        clip.max = e - s;
    }

    return clip;
}

static void list_node_free(list_node_t *head) {
    while(head) {
        list_node_t *next = head->next;

        free(head);
        head = next;
    }
}

intptr_t list_create(int n, intptr_t *datas)
{
    list_head_t *list = list_head_alloc(0);

    if (list) {
        list_node_t *head, *tail;
        int i;

        if (n < 1 || datas == NULL) {
            return (intptr_t) list;
        }

        head = tail = list_node_alloc(datas[0]);
        if (!head) {
            free(list);
            return 0;
        }

        for (i = 1; i < n; i++) {
            list_node_t *node = list_node_alloc(datas[i]);
            if (!node) {
                list_node_free(head);
                free(list);
                return 0;
            }
            node->prev = tail;
            tail->next = node;
            tail = node;
        }

        list->length = i;
        list->head = head;
        list->tail = tail;
    }

    return (intptr_t) list;
}

int list_destroy(intptr_t l)
{
    list_head_t *list = (list_head_t *)l;

    if (list) {
        list_node_free(list->head);
        free(list);
        return 0;
    }

    return -1;
}

int list_length(intptr_t l)
{
    list_head_t *list = (list_head_t *)l;

    if (list) {
        return list->length;
    }
    return -1;
}

int list_get(intptr_t l, int i, intptr_t *d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *node;

    if (list == NULL) {
        return -1;
    }

    node = list_node_get(list, i);
    if (node) {
        *d = node->data;
        return 1;
    } else {
        return 0;
    }
}

int list_set(intptr_t l, int i, intptr_t d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *node;

    if (list == NULL) {
        return -1;
    }

    node = list_node_get(list, i);
    if (node) {
        node->data = d;
        return 1;
    } else {
        return 0;
    }
}

int list_gets(intptr_t l, int start, int n, intptr_t *datas)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *curr;
    int i = 0;

    if (!list || !datas) {
        return -1;
    }

    curr = list_node_get(list, start);
    while(curr && i < n) {
        datas[i++] = curr->data;
        curr = curr->next;
    }

    return i;
}

int list_sets(intptr_t l, int start, int n, intptr_t *datas)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *curr;
    int i = 0;

    if (!list || !datas) {
        return -1;
    }

    curr = list_node_get(list, start);
    while(curr && i < n) {
        curr->data = datas[i++];
        curr = curr->next;
    }

    return i;
}

int list_push(intptr_t l, intptr_t d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *tail;

    if (list == NULL) {
        return -1;
    }

    tail = list_node_alloc(d);
    if (tail == NULL) {
        return -1;
    }

    if (list->head) {
        tail->prev = list->tail;
        list->tail->next = tail;
        list->tail = tail;
    } else {
        list->head = list->tail = tail;
    }
    list->length++;

    return 0;
}

int list_pop(intptr_t l, intptr_t *d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *tail;

    if (list == NULL) {
        return -1;
    }
    tail = list->tail;

    if (tail) {
        list->tail = tail->prev;
        if (list->tail) {
            list->tail->next = NULL;
        }

        *d = tail->data;
        list->length--;

        if (list->tail == NULL) {
            list->head = NULL;
        }

        free(tail);
    }

    return 0;
}

int list_lpush(intptr_t l, intptr_t d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *head;

    if (list == NULL) {
        return -1;
    }

    head = list_node_alloc(d);
    if (head == NULL) {
        return -1;
    }

    if (list->head) {
        head->next = list->head;
        list->head->prev = head;
        list->head = head;
    } else {
        list->head = list->tail = head;
    }
    list->length++;

    return 0;
}

int list_lpop(intptr_t l, intptr_t *d)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *head;

    if (list == NULL) {
        return -1;
    }
    head = list->head;

    if (head) {
        list->head = head->next;
        if (list->head) {
            list->head->prev = NULL;
        }

        *d = head->data;
        list->length--;

        if (list->head == NULL) {
            list->tail = NULL;
        }

        free(head);
    }

    return 0;
}

int list_insert(intptr_t l, int start, int n, intptr_t *datas)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *prev, *next, *head, *tail;
    int i;

    if (list == NULL) {
        return -1;
    }

    if (n == 0) {
        return 0;
    }

    next = list_node_get(list, start);
    if (next) {
        prev = next->prev;
    } else {
        prev = list->tail;
    }

    head = tail = list_node_alloc(datas[0]);
    if (!head) {
        return -1;
    }

    for (i = 1; i < n; i++) {
        list_node_t *node = list_node_alloc(datas[i]);
        if (!node) {
            list_node_free(head);
            return -1;
        }
        node->prev = tail;
        tail->next = node;
        tail = node;
    }

    list->length += i;
    if (prev) {
        head->prev = prev;
        prev->next = head;
    } else {
        list->head = head;
    }

    if (next) {
        tail->next = next;
        next->prev = tail;
    } else {
        list->tail = tail;
    }

    return i;
}

intptr_t list_cut(intptr_t l, int start, int end)
{
    list_head_t *list = (list_head_t *)l;
    list_head_t *list2;
    list_node_t *head, *tail;
    list_clip_t clip;
    int i, max;

    if (!list) {
        return 0;
    }

    clip = list_node_clip(list, start, end);
    head = clip.start;
    max  = clip.max;

    list2 = list_head_alloc(0);
    if (!list2 || !head) {
        return (intptr_t)list2;
    }

    tail = head;
    for (i = 1; i < max && tail->next; i++) {
        tail = tail->next;
    }
    list->length -= i;

    list2->length = i;
    list2->head = head;
    list2->tail = tail;

    if (head->prev == NULL && tail->next == NULL) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        if (head->prev == NULL) {
            list->head = tail->next;
            tail->next->prev = NULL;
            tail->next = NULL;
        } else if (tail->next == NULL) {
            list->tail = head->prev;
            head->prev->next = NULL;
            head->prev = NULL;
        } else {
            head->prev->next = tail->next;
            tail->next->prev = head->prev;
            head->prev = NULL;
            tail->next = NULL;
        }
    }

    return (intptr_t)list2;
}

intptr_t list_slice(intptr_t l, int start, int end)
{
    list_head_t *list = (list_head_t *)l;
    list_head_t *list2 = NULL;
    list_node_t *head = NULL, *tail = NULL, *curr;
    list_clip_t clip;
    int i, max;

    clip = list_node_clip(list, start, end);
    curr = clip.start;
    max  = clip.max;

    if (max) {
        list2 = list_head_alloc(0);
        if (!list2) {
            return 0;
        }

        head = tail = list_node_alloc(curr->data);
        if (!head) {
            free(list2);
            return 0;
        }

        for (i = 1; i < max && curr->next; i++) {
            curr = curr->next;

            list_node_t *node = list_node_alloc(curr->data);
            if (!node) {
                list_node_free(head);
                free(list2);
                return 0;
            }

            node->prev = tail;
            tail->next = node;
            tail = node;
        }

        list2->length = i;
        list2->head = head;
        list2->tail = tail;
    }

    return (intptr_t)list2;
}

intptr_t list_concat(intptr_t a, intptr_t b)
{
    list_head_t *lista = (list_head_t *)a;
    list_head_t *listb = (list_head_t *)b;
    list_head_t *list = NULL;
    list_node_t *head = NULL, *tail = NULL, *curr;

    list = list_head_alloc(0);
    if (list) {
        list_node_t *node;

        if (lista->length) {
            curr = lista->head;

            head = tail = list_node_alloc(curr->data);
            if (!head) {
                free(list);
                return 0;
            }

            curr = curr->next;
            while(curr) {
                node = list_node_alloc(curr->data);
                if (!node) {
                    list_node_free(head);
                    free(list);
                    return 0;
                }

                node->prev = tail;
                tail->next = node;
                tail = node;
                curr = curr->next;
            }
        }
        if (listb->length) {
            curr = listb->head;
            if (!head) {
                head = tail = list_node_alloc(curr->data);
                if (!head) {
                    list_node_free(head);
                    free(list);
                    return 0;
                }
                curr = curr->next;
            }

            while(curr) {
                node = list_node_alloc(curr->data);
                if (!node) {
                    list_node_free(head);
                    free(list);
                    return 0;
                }

                node->prev = tail;
                tail->next = node;
                tail = node;
                curr = curr->next;
            }
        }

        list->length = lista->length + listb->length;
        list->head = head;
        list->tail = tail;
    }

    return (intptr_t)list;
}

intptr_t list_splice(intptr_t list, int start, int end, int n, intptr_t *datas)
{
    intptr_t list2 = list_cut(list, start, end);

    if (list2) {
        list_insert(list, start, n, datas);
    }

    return list2;
}

int list_foreach(intptr_t l, void (*fn)(intptr_t data, void *udata), void *udata)
{
    list_head_t *list = (list_head_t *)l;
    list_node_t *node;

    if (!list || !fn) {
        return -1;
    }

    node = list->head;
    while(node) {
        fn(node->data, udata);
        node = node->next;
    }

    return 0;
}

