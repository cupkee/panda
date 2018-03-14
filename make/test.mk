## GPLv2 License
##
## Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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
			test_lang_image.c \
			test_lang_async.c \
			test_lang_foreign.c

test_CPPFLAGS = -I${BASE}
test_CFLAGS   =
test_LDFLAGS  = -L${BASE}/build/lang -L. -llang -lcunit

VPATH = ${BASE}/cunit:${BASE}/test

include ${BASE}/make/Makefile.pub

