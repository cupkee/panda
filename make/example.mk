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

lib_NAMES = example
bin_NAMES = compile dump repl panda

example_SRCS = sal.c native.c
example_CPPFLAGS = -I${BASE} -Wall -Werror
example_CFLAGS   = -g

compile_SRCS = compile.c
compile_CPPFLAGS = -I${BASE}
compile_CFLAGS   = -g
compile_LDFLAGS  = -L. -L${BASE}/build/lang -lexample -llang

dump_SRCS = dump.c
dump_CPPFLAGS = -I${BASE}
dump_CFLAGS   = -g
dump_LDFLAGS  = -L. -L${BASE}/build/lang -lexample -llang

repl_SRCS = interactive.c
repl_CPPFLAGS = -I${BASE}
repl_CFLAGS   = -g
repl_LDFLAGS  = -L. -L${BASE}/build/lang -lexample -llang -lreadline

panda_SRCS = interpreter.c
panda_CPPFLAGS = -I${BASE}
panda_CFLAGS   = -g
panda_LDFLAGS  = -L. -L${BASE}/build/lang -lexample -llang

VPATH = ${BASE}/example

include ${BASE}/make/Makefile.pub
