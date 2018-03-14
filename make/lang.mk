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

lib_NAMES = lang

lang_SRCS = heap.c \
			ast.c \
			env.c\
			gc.c \
			lex.c \
			val.c \
			parse.c \
			bcode.c \
			compile.c\
			executable.c \
			interp.c \
			type_number.c \
			type_boolean.c \
			type_function.c \
			type_array.c \
			type_string.c \
			type_buffer.c \
			type_object.c \

lang_CPPFLAGS = -I.. -Wall -Wundef
lang_CFLAGS   = -g -Werror
lang_LDFLAGS  =

VPATH = ${BASE}/lang

include ${BASE}/make/Makefile.pub

