
#ifndef __TEST_UTIL_INC__
#define __TEST_UTIL_INC__

#include "lang/ast.h"

void test_clr_line(void);
void test_set_line(const char *line);
int  test_get_line(void *buf, int size);

char * expr_stringify_after_older(expr_t *e, int size, char *buf);

#endif /* __TEST_UTIL_INC__ */

