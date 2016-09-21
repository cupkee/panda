lib_NAMES = cunit
bin_NAMES = test

cunit_SRCS = CUnit_Basic.c \
			 CUnit_Error.c \
			 CUnit_Mem.c \
			 CUnit_TestDB.c \
			 CUnit_TestRun.c \
			 CUnit_Util.c
cunit_CPPFLAGS =
cunit_CFLAGS   =
cunit_LDFLAGS  =

test_SRCS = test.c \
	   		test_util.c \
	   		test_hello.c \
			test_lang_lex.c  \
			test_lang_val.c  \
			test_lang_parse.c \
			test_lang_symtbl.c \
			test_lang_exec.c \
			test_lang_image.c
test_CPPFLAGS = -I..
test_CFLAGS   =
test_LDFLAGS  = -L../lang -L. -llang -lcunit

VPATH = ../cunit
