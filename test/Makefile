bin_NAMES = test

test_SRCS = test.c \
	   		test_util.c \
	   		test_hello.c \
			test_lang_lex.c  \
			test_lang_val.c  \
			test_lang_parse.c \
			test_lang_symtbl.c \
			test_lang_exec.c \
			test_lang_image.c

test_CPPFLAGS = -I${BASE}
test_CFLAGS   =
test_LDFLAGS  = -L${BASE}/sal -L${BASE}/lang -L${BASE}/cunit -lsal -llang -lcunit


