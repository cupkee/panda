/*
 *
 */

#ifndef __STRING_INC__
#define __STRING_INC__

#include <config.h>

typedef void * string_t;

string_t string_create(int size);
string_t string_create2(const CHAR_T *chrs);
string_t string_create3(const char *chrs);
string_t string_create4(const wchar_t *chrs);

int string_destroy(string_t s);

int string_isString(string_t s);
int string_length(string_t s);

int string_get(string_t a, int idx, CHAR_T *chr);
int string_set(string_t a, int idx, CHAR_T chr);

int string_push(string_t a, CHAR_T chr);
int string_pop (string_t a, CHAR_T *chr);
int string_unshift(string_t a, CHAR_T chr);
int string_shift  (string_t a, CHAR_T *chr);
int string_splice(string_t a, int start, int count, int n, const CHAR_T *chrs);
int string_splice2(string_t a, int start, int count, int n, const CHAR_T *chrs);

int string_indexOf(string_t a, CHAR_T chr);
int string_lastIndexOf(string_t a, CHAR_T chr);

int string_foreach(string_t a, int (*fn)(CHAR_T chr));
int string_foreach2(string_t a, int (*fn)(CHAR_T chr, void *), void *udata);

string_t string_concat(string_t a, int n, CHAR_T *chrs);
string_t string_concat2(string_t a, string_t b);
string_t string_slice(string_t a, int start, int end);

#endif /* __STRING_INC__ */
